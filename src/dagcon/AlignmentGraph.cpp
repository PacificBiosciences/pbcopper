#include <pbcopper/dagcon/AlignmentGraph.h>

#include <cassert>
#include <cfloat>
#include <cstdint>

#include <algorithm>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include <boost/format.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/utility/value_init.hpp>

#include <pbcopper/dagcon/Alignment.h>
#include <pbcopper/utility/Ssize.h>

namespace PacBio {
namespace Dagcon {

AlignmentGraph::AlignmentGraph(const std::string& backbone)
    : backboneLength_{Utility::Ssize(backbone)}
{
    // initialize the graph structure with the backbone length + enter/exit
    // vertex
    const size_t backboneLength = backbone.length();
    graph_ = G(backboneLength + 2);
    for (size_t i = 0; i < backboneLength + 1; i++)
        boost::add_edge(i, i + 1, graph_);

    VtxIter curr;
    VtxIter last;
    boost::tie(curr, last) = boost::vertices(graph_);

    enterVtx_ = *curr++;
    graph_[enterVtx_].Base = '^';
    graph_[enterVtx_].Backbone = true;
    for (size_t i = 0; i < backboneLength; ++i, ++curr) {
        VtxDesc v = *curr;
        graph_[v].Backbone = true;
        graph_[v].Weight = 1;
        graph_[v].Base = backbone[i];
        bbMap_[v] = v;
    }
    exitVtx_ = *curr;
    graph_[exitVtx_].Base = '$';
    graph_[exitVtx_].Backbone = true;
}

AlignmentGraph::AlignmentGraph(const size_t backboneLength) : backboneLength_(backboneLength)
{
    graph_ = G(backboneLength + 2);
    for (size_t i = 0; i < backboneLength + 1; i++)
        boost::add_edge(i, i + 1, graph_);

    VtxIter curr;
    VtxIter last;
    boost::tie(curr, last) = boost::vertices(graph_);

    enterVtx_ = *curr++;
    graph_[enterVtx_].Base = '^';
    graph_[enterVtx_].Backbone = true;
    for (size_t i = 0; i < backboneLength; ++i, ++curr) {
        VtxDesc v = *curr;
        graph_[v].Backbone = true;
        graph_[v].Weight = 1;
        graph_[v].Deleted = false;
        graph_[v].Base = 'N';
        bbMap_[v] = v;
    }
    exitVtx_ = *curr;
    graph_[exitVtx_].Base = '$';
    graph_[exitVtx_].Backbone = true;
}

void AlignmentGraph::AddAln(Alignment& alignment, bool useLocalMerge)
{
    const IndexMap index = boost::get(boost::vertex_index, graph_);

    // tracks the position on the backbone
    uint32_t bbPos = alignment.Start;
    VtxDesc prevVtx = enterVtx_;

    if (useLocalMerge) {
        prevVtx = (alignment.Start <= 1) ? enterVtx_ : index[alignment.Start - 1];
    }

    for (size_t i = 0; i < alignment.Query.length(); i++) {
        const char queryBase = alignment.Query[i];
        const char targetBase = alignment.Target[i];
        assert(queryBase != '.');
        assert(targetBase != '.');

        VtxDesc currVtx = index[bbPos];
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
            VtxDesc newVtx = boost::add_vertex(graph_);
            graph_[newVtx].Base = queryBase;
            graph_[newVtx].Weight++;
            graph_[newVtx].Backbone = false;
            graph_[newVtx].Deleted = false;
            bbMap_[newVtx] = bbPos;
            AddEdge(prevVtx, newVtx);
            prevVtx = newVtx;
        }
    }

    AddEdge(prevVtx, exitVtx_);

    if (useLocalMerge) {
        prevVtx = (alignment.End < backboneLength_) ? index[alignment.Start + 1] : exitVtx_;
    }
}

void AlignmentGraph::AddEdge(VtxDesc u, VtxDesc v)
{
    // Check if edge exists with prev node.  If it does, increment edge counter,
    // otherwise add a new edge.
    bool edgeExists = false;
    InEdgeIter ii;
    InEdgeIter ie;
    boost::tie(ii, ie) = boost::in_edges(v, graph_);
    for (; ii != ie; ++ii) {
        const EdgeDesc e = *ii;
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
    std::queue<VtxDesc> seedNodes;
    seedNodes.push(enterVtx_);

    while (true) {
        if (seedNodes.empty()) break;

        const VtxDesc u = seedNodes.front();
        seedNodes.pop();
        MergeInNodes(u);
        MergeOutNodes(u);

        OutEdgeIter oi;
        OutEdgeIter oe;
        boost::tie(oi, oe) = boost::out_edges(u, graph_);
        for (; oi != oe; ++oi) {
            const EdgeDesc e = *oi;
            graph_[e].Visited = true;
            const VtxDesc v = boost::target(e, graph_);

            int notVisited = 0;
            InEdgeIter ii;
            InEdgeIter ie;
            boost::tie(ii, ie) = boost::in_edges(v, graph_);
            for (; ii != ie; ++ii) {
                if (graph_[*ii].Visited == false) notVisited++;
            }

            // move onto the boost::target node after we visit all incoming edges for
            // the boost::target node
            if (notVisited == 0) seedNodes.push(v);
        }
    }
}

void AlignmentGraph::MergeInNodes(VtxDesc node)
{
    std::map<char, std::vector<VtxDesc>> nodeGroups;

    // Group neighboring nodes by base
    InEdgeIter ii;
    InEdgeIter ie;
    boost::tie(ii, ie) = boost::in_edges(node, graph_);
    for (; ii != ie; ++ii) {
        const VtxDesc& inNode = boost::source(*ii, graph_);
        if (out_degree(inNode, graph_) == 1) {
            nodeGroups[graph_[inNode].Base].push_back(inNode);
        }
    }

    // iterate over node groups, merge an accumulate information
    for (const auto& kvp : nodeGroups) {
        const auto& nodes = kvp.second;
        if (nodes.size() <= 1) continue;

        auto ni = nodes.cbegin();
        const VtxDesc an = *ni++;

        OutEdgeIter anoi;
        OutEdgeIter anoe;
        boost::tie(anoi, anoe) = boost::out_edges(an, graph_);

        // Accumulate out edge information
        // IS: This iterates over all alternate nodes with the same base.
        // It only looks at the initial iterator position instead of having
        // another loop from oi to oe. This is fine because there is exactly
        // 1 out edge for each of these nodes.
        for (; ni != nodes.cend(); ++ni) {
            OutEdgeIter oi;
            OutEdgeIter oe;
            boost::tie(oi, oe) = boost::out_edges(*ni, graph_);

            graph_[*anoi].Count += graph_[*oi].Count;  // IS: This sums the EDGE counts.
            graph_[an].Weight += graph_[*ni].Weight;   // IS: This sums the NODE weights.
        }

        // Accumulate in edge information, merges nodes
        ni = nodes.cbegin();
        ++ni;
        for (; ni != nodes.cend(); ++ni) {
            const VtxDesc vtx = *ni;

            InEdgeIter newii;
            InEdgeIter newie;
            boost::tie(newii, newie) = boost::in_edges(vtx, graph_);
            for (; newii != newie; ++newii) {
                const VtxDesc n1 = boost::source(*newii, graph_);

                EdgeDesc e;
                bool exists;
                boost::tie(e, exists) = edge(n1, an, graph_);
                if (exists) {
#if __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
                    graph_[e].Count += graph_[*newii].Count;
#if __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
                } else {
                    const auto p = boost::add_edge(n1, an, graph_);
                    graph_[p.first].Count = graph_[*newii].Count;
                    graph_[p.first].Visited = graph_[*newii].Visited;
                }
            }
            MarkForReaper(vtx);
        }
        MergeInNodes(an);
    }
}

void AlignmentGraph::MergeOutNodes(VtxDesc node)
{
    std::map<char, std::vector<VtxDesc>> nodeGroups;

    OutEdgeIter oi;
    OutEdgeIter oe;
    boost::tie(oi, oe) = boost::out_edges(node, graph_);
    for (; oi != oe; ++oi) {
        const VtxDesc outNode = boost::target(*oi, graph_);
        if (in_degree(outNode, graph_) == 1) {
            nodeGroups[graph_[outNode].Base].push_back(outNode);
        }
    }

    for (const auto& kvp : nodeGroups) {
        const auto& nodes = kvp.second;
        // for (auto kvp = nodeGroups.cbegin(); kvp != nodeGroups.end(); ++kvp) {
        // std::vector<VtxDesc> nodes = (*kvp).second;
        if (nodes.size() <= 1) continue;

        auto ni = nodes.cbegin();
        const VtxDesc an = *ni++;

        // Accumulate inner edge information
        InEdgeIter anii;
        InEdgeIter anie;
        boost::tie(anii, anie) = boost::in_edges(an, graph_);
        for (; ni != nodes.cend(); ++ni) {
            InEdgeIter ii;
            InEdgeIter ie;
            boost::tie(ii, ie) = boost::in_edges(*ni, graph_);

            graph_[*anii].Count += graph_[*ii].Count;
            graph_[an].Weight += graph_[*ni].Weight;
        }

        // Accumulate and merge outer edge information
        ni = nodes.cbegin();
        ++ni;
        for (; ni != nodes.cend(); ++ni) {
            const VtxDesc vtx = *ni;

            OutEdgeIter newoi;
            OutEdgeIter newoe;
            boost::tie(newoi, newoe) = boost::out_edges(vtx, graph_);
            for (; newoi != newoe; ++newoi) {
                const VtxDesc n2 = boost::target(*newoi, graph_);

                EdgeDesc e;
                bool exists;
                boost::tie(e, exists) = edge(an, n2, graph_);
                if (exists) {
#if __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
                    graph_[e].Count += graph_[*newoi].Count;
#if __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
                } else {
                    const auto p = boost::add_edge(an, n2, graph_);
                    graph_[p.first].Count = graph_[*newoi].Count;
                    graph_[p.first].Visited = graph_[*newoi].Visited;
                }
            }
            MarkForReaper(vtx);
        }
    }
}

void AlignmentGraph::MarkForReaper(VtxDesc node)
{
    graph_[node].Deleted = true;
    clear_vertex(node, graph_);
    reaperBag_.push_back(node);
}

void AlignmentGraph::ReapNodes()
{
    int reapCount = 0;
    std::sort(reaperBag_.begin(), reaperBag_.end());
    for (const auto& curr : reaperBag_) {
        assert(graph_[curr].Backbone == false);
        remove_vertex(curr - reapCount, graph_);
        ++reapCount;
    }
}

std::string AlignmentGraph::Consensus(int minWeight)
{
    // get the best scoring path
    const auto& path = BestPath();

    // consensus sequence
    std::string cns;

    // track the longest consensus path meeting minimum weight
    int offs = 0;
    int bestOffs = 0;
    int length = 0;
    int idx = 0;
    bool metWeight = false;
    for (const auto& n : path) {
        if ((n.Base == graph_[enterVtx_].Base) || (n.Base == graph_[exitVtx_].Base)) {
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

    // get the best scoring path
    const auto path = BestPath();

    // consensus sequence
    std::string cns;

    // track the longest consensus path meeting minimum weight
    int offs = 0;
    int idx = 0;
    bool metWeight = false;
    for (const auto& n : path) {
        if ((n.Base == graph_[enterVtx_].Base) || (n.Base == graph_[exitVtx_].Base)) {
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
    const int64_t pathLength = Utility::Ssize(path);
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
    for (int64_t i = Utility::Ssize(path) - 1; i >= 0; --i, ++numClippedBack) {
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
        if ((n.Base == graph_[enterVtx_].Base) || (n.Base == graph_[exitVtx_].Base)) {
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

std::vector<AlignmentNode> AlignmentGraph::BestPath()
{
    EdgeIter ei;
    EdgeIter ee;
    boost::tie(ei, ee) = edges(graph_);
    for (; ei != ee; ++ei)
        graph_[*ei].Visited = false;

    std::map<VtxDesc, EdgeDesc> bestNodeScoreEdge;
    std::map<VtxDesc, float> nodeScore;
    std::queue<VtxDesc> seedNodes;

    // start at the end and make our way backwards
    seedNodes.push(exitVtx_);
    nodeScore[exitVtx_] = 0.0f;

    while (true) {
        if (seedNodes.size() == 0) break;

        const VtxDesc n = seedNodes.front();
        seedNodes.pop();

        bool bestEdgeFound = false;
        float bestScore = -FLT_MAX;
        EdgeDesc bestEdgeD = boost::initialized_value;

        OutEdgeIter oi;
        OutEdgeIter oe;
        boost::tie(oi, oe) = boost::out_edges(n, graph_);
        for (; oi != oe; ++oi) {
            const EdgeDesc& outEdgeD = *oi;
            const VtxDesc outNodeD = boost::target(outEdgeD, graph_);
            const AlignmentNode& outNode = graph_[outNodeD];
            float newScore;
            float score = nodeScore[outNodeD];
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

        InEdgeIter ii;
        InEdgeIter ie;
        boost::tie(ii, ie) = boost::in_edges(n, graph_);
        for (; ii != ie; ++ii) {
            const EdgeDesc& inEdge = *ii;
            graph_[inEdge].Visited = true;
            const VtxDesc inNode = boost::source(inEdge, graph_);
            int notVisited = 0;

            OutEdgeIter newoi;
            OutEdgeIter newoe;
            boost::tie(newoi, newoe) = boost::out_edges(inNode, graph_);
            for (; newoi != newoe; ++newoi) {
                if (graph_[*newoi].Visited == false) notVisited++;
            }

            // move onto the target node after we visit all incoming edges for
            // the target node
            if (notVisited == 0) seedNodes.push(inNode);
        }
    }

    // construct the final best path
    VtxDesc prev = enterVtx_;
    VtxDesc next;
    std::vector<AlignmentNode> bpath;
    while (true) {
        bpath.push_back(graph_[prev]);
        if (bestNodeScoreEdge.count(prev) == 0) {
            break;
        } else {
            const EdgeDesc& bestOutEdge = bestNodeScoreEdge[prev];
            graph_[prev].BestOutEdge = bestOutEdge;
            next = boost::target(bestOutEdge, graph_);
            graph_[next].BestInEdge = bestOutEdge;
            prev = next;
        }
    }

    return bpath;
}

void AlignmentGraph::PrintGraph(std::ostream& out)
{
    ReapNodes();
    boost::write_graphviz(out, graph_, make_label_writer(get(&AlignmentNode::Base, graph_)),
                          make_label_writer(get(&AlignmentEdge::Count, graph_)));
}

bool AlignmentGraph::DanglingNodes()
{
    VtxIter curr;
    VtxIter last;
    boost::tie(curr, last) = boost::vertices(graph_);
    for (; curr != last; ++curr) {
        if (graph_[*curr].Deleted) continue;
        if ((graph_[*curr].Base == graph_[enterVtx_].Base) ||
            (graph_[*curr].Base == graph_[exitVtx_].Base)) {
            continue;
        }

        const auto indeg = out_degree(*curr, graph_);
        const auto outdeg = in_degree(*curr, graph_);
        if ((outdeg > 0) && (indeg > 0)) continue;

        return true;
    }
    return false;
}

}  // namespace Dagcon
}  // namespace PacBio
