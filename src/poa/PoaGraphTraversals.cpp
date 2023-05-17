#include <pbcopper/poa/PoaGraph.h>

#include "PoaGraphImpl.h"
#include "VectorL.h"

#include <boost/foreach.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/unordered_set.hpp>

#include <limits>
#include <sstream>

#include <cstddef>

namespace PacBio {
namespace Poa {
namespace detail {

std::string sequenceAlongPath(const BoostGraph& /*unused*/, const VertexInfoMap& vertexInfoMap,
                              const std::vector<VD>& path)
{
    std::ostringstream ss;
    for (const VD v : path) {
        ss << vertexInfoMap[v].Base;
    }
    return ss.str();
}

boost::unordered_set<VD> SpanningDFS(const VD start, const VD end, const BoostGraph& g)
{
    std::vector<VD> stack;
    boost::unordered_set<VD> fwd;
    boost::unordered_set<VD> rev;
    // find all vertices reachable from start
    stack.push_back(start);
    do {
        VD v = stack.back();
        stack.pop_back();
        // mark those we've already visited,
        //   if so, skip
        if (fwd.find(v) != fwd.end()) {
            continue;
        }
        fwd.insert(v);
        BOOST_FOREACH (const ED& e, out_edges(v, g)) {
            stack.push_back(target(e, g));
        }
    } while (!stack.empty());
    // find all vertices that can reach end
    //   FROM start
    stack.push_back(end);
    do {
        VD v = stack.back();
        stack.pop_back();
        // if it's not been visited in the forward pass,
        //   or we've already visited it here, skip
        if (fwd.find(v) == fwd.end() || rev.find(v) != rev.end()) {
            continue;
        }
        rev.insert(v);
        BOOST_FOREACH (const ED& e, in_edges(v, g)) {
            stack.push_back(source(e, g));
        }
    } while (!stack.empty());
    return rev;
}

std::vector<VD> PoaGraphImpl::sortedVertices() const
{
    std::vector<VD> sv(num_vertices(g_));
    topological_sort(g_, sv.rbegin());
    return sv;
}

void PoaGraphImpl::tagSpan(VD start, VD end)
{
    boost::unordered_set<VD> vertices = SpanningDFS(start, end, g_);
    for (const VD v : vertices) {
        vertexInfoMap_[v].SpanningReads++;
    }
}

std::vector<VD> PoaGraphImpl::consensusPath(Align::AlignMode mode, int minCoverage) const
{
    // Pat's note on the approach here:
    //
    // "A node gets a score of NumReads if all reads go through
    //  it, and a score of -NumReads if no reads go through it The
    //  shift of -0.0001 breaks ties in favor of skipping
    //  half-full nodes.  In the 2 reads case this will get rid of
    //  insertions which are the more common error."
    //
    // The interpretation of minCoverage (which is applicable only
    // for LOCAL, SEMIGLOBAL modes) is that it represents
    // application-specific knowledge of the basal coverage level
    // of reads in the template, such that if a node is contained
    // in fewer than minCoverage reads, it will be penalized
    // against inclusion in the consensus.
    int totalReads = NumReads();

    std::list<VD> path;
    std::list<VD> sortedVerticesLocal(num_vertices(g_));
    topological_sort(g_, sortedVerticesLocal.rbegin());
    boost::unordered_map<VD, VD> bestPrevVertex;

    // ignore ^ and $
    // TODO(dalexander): find a cleaner way to do this
    vertexInfoMap_[sortedVerticesLocal.front()].ReachingScore = 0;
    sortedVerticesLocal.pop_back();
    sortedVerticesLocal.pop_front();

    VD bestVertex = null_vertex;
    float bestReachingScore = -FLT_MAX;
    for (const VD v : sortedVerticesLocal) {
        PoaNode& vInfo = vertexInfoMap_[v];
        int containingReads = vInfo.Reads;
        int spanningReads = vInfo.SpanningReads;
        float score =
            (mode != Align::AlignMode::GLOBAL)
                ? (2 * containingReads - 1 * std::max(spanningReads, minCoverage) - 0.0001F)
                : (2 * containingReads - 1 * totalReads - 0.0001F);
        vInfo.Score = score;
        vInfo.ReachingScore = score;
        bestPrevVertex[v] = null_vertex;
        for (const ED& e : inEdges(v, g_)) {
            VD sourceVertex = source(e, g_);
            float rsc = score + vertexInfoMap_[sourceVertex].ReachingScore;
            if (rsc > vInfo.ReachingScore) {
                vInfo.ReachingScore = rsc;
                bestPrevVertex[v] = sourceVertex;
            }
            if (rsc > bestReachingScore) {
                bestVertex = v;
                bestReachingScore = rsc;
            }
            // if the score is the same, the order we've encountered vertices
            //   might not be deterministic. Fix this by comparing on
            //   vertex_index
            else if ((rsc == bestReachingScore) &&
                     (get(boost::vertex_index, g_, v) < get(boost::vertex_index, g_, bestVertex))) {
                bestVertex = v;
            }
        }
    }
    assert(bestVertex != null_vertex);

    // trace back from best-scoring vertex
    VD v = bestVertex;
    while (v != null_vertex) {
        path.push_front(v);
        v = bestPrevVertex[v];
    }
    return std::vector<VD>(path.begin(), path.end());
}

void PoaGraphImpl::threadFirstRead(std::string sequence, std::vector<Vertex>* outputPath)
{
    // first sequence in the alignment
    VD u = null_vertex;
    VD v;
    VD startSpanVertex = null_vertex;
    VD endSpanVertex;
    int readPos = 0;

    if (outputPath) {
        outputPath->clear();
    }

    for (const char base : sequence) {
        v = addVertex(base);
        if (outputPath) {
            outputPath->push_back(externalize(v));
        }
        if (readPos == 0) {
            add_edge(enterVertex_, v, g_);
            startSpanVertex = v;
        } else {
            add_edge(u, v, g_);
        }
        u = v;
        readPos++;
    }
    assert(startSpanVertex != null_vertex);
    assert(u != null_vertex);
    endSpanVertex = u;
    add_edge(u, exitVertex_, g_);  // terminus -> $
    tagSpan(startSpanVertex, endSpanVertex);
}

void PoaGraphImpl::tracebackAndThread(std::string sequence,
                                      const AlignmentColumnMap& alignmentColumnForVertex,
                                      Align::AlignMode alignMode, std::vector<Vertex>* outputPath)
{
    const int I = sequence.length();

    // perform traceback from (I,$), threading the new sequence into
    // the graph as we go.
    int i = I;
    VD v = null_vertex;
    VD forkVertex = null_vertex;
    VD u = exitVertex_;
    int span = 0;
    VD startSpanVertex;
    VD endSpanVertex = alignmentColumnForVertex.at(exitVertex_)->PreviousVertex[I];

    if (outputPath) {
        outputPath->resize(I);
        std::fill(outputPath->begin(), outputPath->end(), std::numeric_limits<std::size_t>::max());
    }

#define READPOS (i - 1)
#define VERTEX_ON_PATH(readPos, v)                 \
    if (outputPath) {                              \
        (*outputPath)[(readPos)] = externalize(v); \
    }

    while (!(u == enterVertex_ && i == 0)) {
        // u -> v
        // u: current vertex
        // v: vertex last visited in traceback (could be == u)
        // forkVertex: the vertex that will be the target of a new edge

        /* Vertex uExt = */ this->externalize(u);  // DEBUGGING
        /* Vertex vExt = */ this->externalize(v);  // DEBUGGING

        const AlignmentColumn* const curCol = alignmentColumnForVertex.at(u).get();
        assert(curCol != nullptr);
        PoaNode& curNodeInfo = vertexInfoMap_[u];
        VD prevVertex = curCol->PreviousVertex[i];
        MoveType reachingMove = curCol->ReachingMove[i];

        if (reachingMove == StartMove) {
            assert(v != null_vertex);

            if (forkVertex == null_vertex) {
                forkVertex = v;
            }
            // In local model thread read bases, adjusting i (should stop at 0)
            while (i > 0) {
                assert(alignMode == Align::AlignMode::LOCAL);
                VD newForkVertex = addVertex(sequence[READPOS], 1, span);
                add_edge(newForkVertex, forkVertex, g_);
                VERTEX_ON_PATH(READPOS, newForkVertex);
                forkVertex = newForkVertex;
                i--;
            }
        } else if (reachingMove == EndMove) {
            assert(forkVertex == null_vertex && u == exitVertex_ && v == null_vertex);

            forkVertex = exitVertex_;

            if (alignMode == Align::AlignMode::LOCAL) {
                // Find the row # we are coming from, walk
                // back to there, threading read bases onto
                // graph via forkVertex, adjusting i.
                const AlignmentColumn* const prevCol =
                    alignmentColumnForVertex.at(prevVertex).get();
                int prevRow = ArgMax(prevCol->Score);

                while (i > prevRow) {
                    VD newForkVertex = addVertex(sequence[READPOS], 1, span);
                    add_edge(newForkVertex, forkVertex, g_);
                    VERTEX_ON_PATH(READPOS, newForkVertex);
                    forkVertex = newForkVertex;
                    i--;
                }
            }
        } else if (reachingMove == MatchMove) {
            VERTEX_ON_PATH(READPOS, u);
            // if there is an extant forkVertex, join it
            if (forkVertex != null_vertex) {
                add_edge(u, forkVertex, g_);
                forkVertex = null_vertex;
            }
            // add to existing node
            curNodeInfo.Reads++;
            i--;
        } else if (reachingMove == DeleteMove) {
            if (forkVertex == null_vertex) {
                forkVertex = v;
            }
        } else if (reachingMove == ExtraMove || reachingMove == MismatchMove) {
            // begin a new arc with this read base
            VD newForkVertex = addVertex(sequence[READPOS], 1, span);
            if (forkVertex == null_vertex) {
                forkVertex = v;
            }
            add_edge(newForkVertex, forkVertex, g_);
            VERTEX_ON_PATH(READPOS, newForkVertex);
            forkVertex = newForkVertex;
            i--;
        } else {
            throw std::runtime_error("unreachable");
        }

        v = u;
        u = prevVertex;
        span = curNodeInfo.SpanningReads;
    }
    startSpanVertex = v;

    // if there is an extant forkVertex, join it to enterVertex
    if (forkVertex != null_vertex) {
        add_edge(enterVertex_, forkVertex, g_);
        startSpanVertex = forkVertex;
        forkVertex = null_vertex;
    }

    if (startSpanVertex != exitVertex_) {
        tagSpan(startSpanVertex, endSpanVertex);
    }

    // all filled in?
    assert(outputPath == nullptr ||
           std::find(outputPath->begin(), outputPath->end(),
                     std::numeric_limits<std::size_t>::max()) == outputPath->end());

#undef READPOS
#undef VERTEX_ON_PATH
}

}  // namespace detail
}  // namespace Poa
}  // namespace PacBio
