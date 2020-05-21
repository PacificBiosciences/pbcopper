#include <pbcopper/dagcon/AlignmentGraph.h>

#include <cassert>
#include <cfloat>
#include <cstdint>

#include <algorithm>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/utility/value_init.hpp>

#include <pbcopper/dagcon/Alignment.h>
#include <pbcopper/utility/Ssize.h>

namespace PacBio {
namespace Dagcon {

AlignmentGraph::AlignmentGraph(const std::string& backbone) : backboneLength_{backbone.length()}
{
    // initialize the graph structure with the backbone length + enter/exit vertex
    graph_ = GraphType{backboneLength_ + 2};
    for (size_t i = 0; i < backboneLength_ + 1; ++i)
        boost::add_edge(i, i + 1, graph_);

    VertexIterator curr;
    VertexIterator last;
    boost::tie(curr, last) = boost::vertices(graph_);

    // enter vertex
    enterVertex_ = *curr;
    graph_[enterVertex_].Base = '^';
    graph_[enterVertex_].Backbone = true;
    ++curr;

    // internal vertices
    for (size_t i = 0; i < backboneLength_; ++i) {
        VertexIndex v = *curr;
        graph_[v].Backbone = true;
        graph_[v].Weight = 1;
        graph_[v].Base = backbone[i];
        bbMap_[v] = v;
        ++curr;
    }

    // ext vertex
    exitVertex_ = *curr;
    graph_[exitVertex_].Base = '$';
    graph_[exitVertex_].Backbone = true;
}

AlignmentGraph::AlignmentGraph(const size_t backboneLength)
    : AlignmentGraph{std::string(backboneLength, 'N')}
{
}

void AlignmentGraph::AddAlignment(Alignment& alignment, bool useLocalMerge)
{
    const IndexMap index = boost::get(boost::vertex_index, graph_);
    // tracks the position on the backbone
    uint32_t bbPos = alignment.Start;
    VertexIndex prevVtx = enterVertex_;

    if (useLocalMerge) {
        prevVtx = (alignment.Start <= 1) ? enterVertex_ : index[alignment.Start - 1];
    }

    for (size_t i = 0; i < alignment.Query.length(); i++) {
        const char queryBase = alignment.Query[i];
        const char targetBase = alignment.Target[i];
        assert(queryBase != '.');
        assert(targetBase != '.');

        VertexIndex currVtx = index[bbPos];
        // match
        if (queryBase == targetBase) {
            graph_[bbMap_[currVtx]].Coverage++;

            // NOTE: for empty backbones
            graph_[bbMap_[currVtx]].Base = targetBase;

            graph_[currVtx].Weight++;
            AddEdge(prevVtx, currVtx);
            bbPos++;
            prevVtx = currVtx;
            // query deletion
        } else if (queryBase == '-' && targetBase != '-') {
            graph_[bbMap_[currVtx]].Coverage++;

            // NOTE: for empty backbones
            graph_[bbMap_[currVtx]].Base = targetBase;

            bbPos++;
            // query insertion
        } else if (queryBase != '-' && targetBase == '-') {
            // create new node and edge
            const VertexIndex newVtx = boost::add_vertex(graph_);
            graph_[newVtx].Base = queryBase;
            graph_[newVtx].Weight++;
            graph_[newVtx].Backbone = false;
            graph_[newVtx].Deleted = false;
            bbMap_[newVtx] = bbPos;
            AddEdge(prevVtx, newVtx);
            prevVtx = newVtx;
        }
    }

    AddEdge(prevVtx, exitVertex_);

    if (useLocalMerge) {
        prevVtx = (alignment.End < backboneLength_) ? index[alignment.Start + 1] : exitVertex_;
    }
}

void AlignmentGraph::AddEdge(VertexIndex u, VertexIndex v)
{
    // Check if edge exists with prev node.  If it does, increment edge counter,
    // otherwise add a new edge.
    bool edgeExists = false;

    for (const auto& e : boost::make_iterator_range(boost::in_edges(v, graph_))) {
        if (boost::source(e, graph_) == u) {
            // increment edge count
            graph_[e].Count++;
            edgeExists = true;
        }
    }
    if (!edgeExists) {
        // add new edge
        const auto p = boost::add_edge(u, v, graph_);
        graph_[p.first].Count++;
    }
}

void AlignmentGraph::MergeNodes()
{
    std::queue<VertexIndex> seedNodes;
    seedNodes.push(enterVertex_);

    while (true) {
        if (seedNodes.empty()) break;

        const VertexIndex u = seedNodes.front();
        seedNodes.pop();
        MergeInNodes(u);
        MergeOutNodes(u);

        for (const auto& e : boost::make_iterator_range(boost::out_edges(u, graph_))) {
            graph_[e].Visited = true;

            int notVisited = 0;
            const VertexIndex v = boost::target(e, graph_);
            for (const auto& inEdge : boost::make_iterator_range(boost::in_edges(v, graph_))) {
                if (graph_[inEdge].Visited == false) ++notVisited;
            }

            // move onto the boost::target node after we visit all incoming edges for
            // the boost::target node
            if (notVisited == 0) seedNodes.push(v);
        }
    }
}

void AlignmentGraph::MergeInNodes(VertexIndex n)
{
    std::map<char, std::vector<VertexIndex>> nodeGroups;

    for (const auto& inEdge : boost::make_iterator_range(boost::in_edges(n, graph_))) {
        const VertexIndex inNode = boost::source(inEdge, graph_);
        if (out_degree(inNode, graph_) == 1) {
            nodeGroups[graph_[inNode].Base].push_back(inNode);
        }
    }

    // iterate over node groups, merge an accumulate information
    for (const auto& kvp : nodeGroups) {
        const auto& nodes = kvp.second;
        if (nodes.size() <= 1) continue;

        auto ni = nodes.cbegin();
        const VertexIndex an = *ni++;

        // Accumulate out edge information
        // IS: This iterates over all alternate nodes with the same base.
        // It only looks at the initial iterator position instead of having
        // another loop from oi to oe. This is fine because there is exactly
        // 1 out edge for each of these nodes.
        OutEdgeIterator anoi;
        OutEdgeIterator anoe;
        boost::tie(anoi, anoe) = boost::out_edges(an, graph_);
        for (; ni != nodes.cend(); ++ni) {

            OutEdgeIterator oi;
            OutEdgeIterator oe;
            boost::tie(oi, oe) = boost::out_edges(*ni, graph_);

            graph_[*anoi].Count += graph_[*oi].Count;  // IS: This sums the EDGE counts.
            graph_[an].Weight += graph_[*ni].Weight;   // IS: This sums the NODE weights.
        }

        // Accumulate in edge information, merges nodes
        ni = nodes.cbegin();
        ++ni;
        for (; ni != nodes.cend(); ++ni) {
            const VertexIndex vtx = *ni;
            for (const auto& newEdge : boost::make_iterator_range(boost::in_edges(vtx, graph_))) {
                const VertexIndex n1 = boost::source(newEdge, graph_);

                EdgeIndex e;
                bool exists;
                boost::tie(e, exists) = edge(n1, an, graph_);
                if (exists) {
#if __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
                    graph_[e].Count += graph_[newEdge].Count;
#if __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
                } else {
                    const auto p = boost::add_edge(n1, an, graph_);
                    graph_[p.first].Count = graph_[newEdge].Count;
                    graph_[p.first].Visited = graph_[newEdge].Visited;
                }
            }
            MarkForReaper(vtx);
        }
        MergeInNodes(an);
    }
}

void AlignmentGraph::MergeOutNodes(VertexIndex n)
{
    std::map<char, std::vector<VertexIndex>> nodeGroups;

    for (const auto& outEdge : boost::make_iterator_range(boost::out_edges(n, graph_))) {
        const VertexIndex outNode = boost::target(outEdge, graph_);
        if (in_degree(outNode, graph_) == 1) {
            nodeGroups[graph_[outNode].Base].push_back(outNode);
        }
    }

    for (const auto& kvp : nodeGroups) {
        const auto& nodes = kvp.second;
        if (nodes.size() <= 1) continue;

        auto nodeIter = nodes.cbegin();
        const VertexIndex an = *nodeIter;
        ++nodeIter;

        // Accumulate inner edge information
        InEdgeIterator anii;
        InEdgeIterator anie;
        boost::tie(anii, anie) = boost::in_edges(an, graph_);
        for (; nodeIter != nodes.cend(); ++nodeIter) {
            InEdgeIterator ii;
            InEdgeIterator ie;
            boost::tie(ii, ie) = boost::in_edges(*nodeIter, graph_);
            graph_[*anii].Count += graph_[*ii].Count;
            graph_[an].Weight += graph_[*nodeIter].Weight;
        }

        // Accumulate and merge outer edge information
        nodeIter = nodes.cbegin();
        ++nodeIter;
        for (; nodeIter != nodes.cend(); ++nodeIter) {
            VertexIndex vtx = *nodeIter;
            for (const auto& newEdge : boost::make_iterator_range(boost::out_edges(vtx, graph_))) {
                const VertexIndex n2 = boost::target(newEdge, graph_);

                EdgeIndex e;
                bool exists;
                boost::tie(e, exists) = edge(an, n2, graph_);
                if (exists) {
#if __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
                    graph_[e].Count += graph_[newEdge].Count;
#if __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
                } else {
                    const auto p = boost::add_edge(an, n2, graph_);
                    graph_[p.first].Count = graph_[newEdge].Count;
                    graph_[p.first].Visited = graph_[newEdge].Visited;
                }
            }
            MarkForReaper(vtx);
        }
    }
}

void AlignmentGraph::MarkForReaper(VertexIndex node)
{
    graph_[node].Deleted = true;
    clear_vertex(node, graph_);
    reaperBag_.push_back(node);
}

void AlignmentGraph::ReapNodes()
{
    int reapCount = 0;
    std::sort(reaperBag_.begin(), reaperBag_.end());
    for (auto& node : reaperBag_) {
        assert(graph_[node].Backbone == false);
        remove_vertex(node - reapCount, graph_);
        ++reapCount;
    }
}

std::string AlignmentGraph::Consensus(int minWeight)
{
    // consensus sequence
    std::string cns;

    // track the longest consensus path meeting minimum weight
    int offs = 0;
    int bestOffs = 0;
    int length = 0;
    int idx = 0;
    bool metWeight = false;
    for (const auto& n : BestPath()) {
        if (n.Base == graph_[enterVertex_].Base || n.Base == graph_[exitVertex_].Base) {
            continue;
        }

        cns += n.Base;

        // initial beginning of minimum weight section
        if (!metWeight && n.Weight >= minWeight) {
            offs = idx;
            metWeight = true;
        } else if (metWeight && n.Weight < minWeight) {
            // concluded minimum weight section, update if longest seen so far
            if ((idx - offs) > length) {
                bestOffs = offs;
                length = idx - offs;
            }
            metWeight = false;
        }
        idx++;
    }

    // include end of sequence
    if (metWeight && (idx - offs) > length) {
        bestOffs = offs;
        length = idx - offs;
    }

    return cns.substr(bestOffs, length);
}

void AlignmentGraph::Consensus(std::vector<ConsensusResult>& seqs, int minWeight, size_t minLen)
{
    seqs.clear();

    // consensus sequence
    std::string cns;

    // track the longest consensus path meeting minimum weight
    int offs = 0;
    int idx = 0;
    bool metWeight = false;
    for (const auto& n : BestPath()) {
        if (n.Base == graph_[enterVertex_].Base || n.Base == graph_[exitVertex_].Base) {
            continue;
        }

        cns += n.Base;

        // initial beginning of minimum weight section
        if (!metWeight && n.Weight >= minWeight) {
            offs = idx;
            metWeight = true;
        } else if (metWeight && n.Weight < minWeight) {
            // concluded minimum weight section, add sequence to supplied vector
            metWeight = false;
            const size_t length = idx - offs;
            if (length >= minLen) {
                seqs.emplace_back(ConsensusResult{{offs, idx}, cns.substr(offs, length)});
            }
        }
        idx++;
    }

    // include end of sequence
    if (metWeight) {
        const size_t length = idx - offs;
        if (length >= minLen) {
            seqs.emplace_back(ConsensusResult{{offs, idx}, cns.substr(offs, length)});
        }
    }
}

void AlignmentGraph::ConsensusWithMinFlankCoverage(std::vector<ConsensusResult>& seqs,
                                                   int minWeight, int minFlankCoverage,
                                                   size_t minLen)
{
    seqs.clear();

    // get the best scoring path
    const auto path = BestPath();
    const auto pathLength = Utility::Ssize(path);
    if (pathLength == 0) {
        return;
    }

    int64_t numClippedFront = 0;
    auto startNode = path.begin();
    for (; startNode != path.end(); ++startNode, ++numClippedFront) {
        const AlignmentNode& n = *startNode;
        if (n.Coverage >= minFlankCoverage) {
            break;
        }
    }

    auto endNode = path.end();
    int64_t numClippedBack = 0;
    for (int64_t i = pathLength - 1; i >= 0; --i, ++numClippedBack) {
        if (path[i].Coverage >= minFlankCoverage) {
            break;
        }
        --endNode;
    }

    if (numClippedFront >= (pathLength - numClippedBack)) {
        return;
    }

    // consensus sequence
    std::string cns;

    // track the longest consensus path meeting minimum weight
    int offs = 0;
    int idx = 0;
    bool metWeight = false;
    auto curr = startNode;
    for (; curr != endNode; ++curr) {
        const AlignmentNode& n = *curr;
        if (n.Base == graph_[enterVertex_].Base || n.Base == graph_[exitVertex_].Base) continue;

        cns += n.Base;

        // initial beginning of minimum weight section
        if (!metWeight && n.Weight >= minWeight) {
            offs = idx;
            metWeight = true;
        } else if (metWeight && n.Weight < minWeight) {
            // concluded minimum weight section, add sequence to supplied vector
            metWeight = false;
            const size_t length = idx - offs;
            if (length >= minLen) {
                seqs.emplace_back(ConsensusResult{{offs, idx}, cns.substr(offs, length)});
            }
        }
        idx++;
    }
    // include end of sequence
    if (metWeight) {
        const size_t length = idx - offs;
        if (length >= minLen) {
            seqs.emplace_back(ConsensusResult{{offs, idx}, cns.substr(offs, length)});
        }
    }
}

std::vector<AlignmentNode> AlignmentGraph::BestPath()
{
    for (const auto& edge : boost::make_iterator_range(edges(graph_))) {
        graph_[edge].Visited = false;
    }

    std::map<VertexIndex, EdgeIndex> bestNodeScoreEdge;
    std::map<VertexIndex, float> nodeScore;
    std::queue<VertexIndex> seedNodes;

    // start at the end and make our way backwards
    seedNodes.push(exitVertex_);
    nodeScore[exitVertex_] = 0.0f;

    while (true) {
        if (seedNodes.empty()) break;

        const VertexIndex n = seedNodes.front();
        seedNodes.pop();

        bool bestEdgeFound = false;
        float bestScore = -FLT_MAX;
        EdgeIndex bestEdgeD = boost::initialized_value;

        for (const auto& outEdgeD : boost::make_iterator_range(boost::out_edges(n, graph_))) {
            const VertexIndex outNodeD = boost::target(outEdgeD, graph_);
            const AlignmentNode& outNode = graph_[outNodeD];

            float score = nodeScore[outNodeD];
            float newScore = 0.0f;
            if (outNode.Backbone && outNode.Weight == 1) {
                newScore = score - 10.0f;
            } else {
                const AlignmentNode& bbNode = graph_[bbMap_[outNodeD]];
                newScore = graph_[outEdgeD].Count - bbNode.Coverage * 0.5f + score;
            }

            if (newScore > bestScore) {
                bestScore = newScore;
                bestEdgeD = outEdgeD;
                bestEdgeFound = true;
            }
        }

        if (bestEdgeFound) {
            nodeScore[n] = bestScore;
            bestNodeScoreEdge[n] = bestEdgeD;
        }

        for (const auto& inEdge : boost::make_iterator_range(boost::in_edges(n, graph_))) {
            graph_[inEdge].Visited = true;
            const VertexIndex inNode = boost::source(inEdge, graph_);
            int notVisited = 0;

            for (const auto& newOutEdge :
                 boost::make_iterator_range(boost::out_edges(inNode, graph_))) {
                if (graph_[newOutEdge].Visited == false) notVisited++;
            }

            // move onto the target node after we visit all incoming edges for
            // the target node
            if (notVisited == 0) seedNodes.push(inNode);
        }
    }

    // construct the final best path
    VertexIndex prev = enterVertex_;
    VertexIndex next;
    std::vector<AlignmentNode> bpath;
    while (true) {
        bpath.push_back(graph_[prev]);
        if (bestNodeScoreEdge.count(prev) == 0) {
            break;
        } else {
            const EdgeIndex bestOutEdge = bestNodeScoreEdge[prev];
            graph_[prev].BestOutEdge = bestOutEdge;
            next = boost::target(bestOutEdge, graph_);
            graph_[next].BestInEdge = bestOutEdge;
            prev = next;
        }
    }

    return bpath;
}

bool AlignmentGraph::DanglingNodes()
{
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(graph_))) {
        if (graph_[vertex].Deleted) continue;
        if (graph_[vertex].Base == graph_[enterVertex_].Base ||
            graph_[vertex].Base == graph_[exitVertex_].Base) {
            continue;
        }

        const int indeg = out_degree(vertex, graph_);
        const int outdeg = in_degree(vertex, graph_);
        if (outdeg > 0 && indeg > 0) continue;

        return true;
    }
    return false;
}

}  // namespace Dagcon
}  // namespace PacBio
