#include "PoaGraphImpl.h"

#include <pbcopper/align/AlignConfig.h>
#include <pbcopper/poa/PoaConsensus.h>
#include <pbcopper/poa/PoaGraph.h>
#include <pbcopper/poa/RangeFinder.h>

#include <boost/foreach.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>

#include <fstream>
#include <ostream>
#include <set>
#include <sstream>

namespace boost {

class my_label_writer
{
public:
    explicit my_label_writer(PacBio::Poa::detail::VertexInfoMap map, bool color, bool verbose,
                             const PacBio::Poa::PoaConsensus* pc = nullptr)
        : map_(map), cssVtxs_(), color_(color), verbose_(verbose)
    {
        if (pc != nullptr) {
            cssVtxs_.insert(pc->Path.begin(), pc->Path.end());
        }
    }

    template <class descriptor>
    void operator()(std::ostream& out, const descriptor& v) const
    {
        PacBio::Poa::PoaGraph::Vertex vertexId = map_[v].Id;

        std::string nodeColoringAttribute =
            (color_ && isInConsensus(vertexId) ? R"( style="filled", fillcolor="lightblue" ,)"
                                               : "");

        if (!verbose_) {
            out << format("[shape=Mrecord,%s label=\"{ %c | %d }\"]") % nodeColoringAttribute %
                       map_[v].Base % map_[v].Reads;
        } else {
            out << format(
                       "[shape=Mrecord,%s label=\"{ "
                       "{ %d | %c } | "
                       "{ %d | %d } | "
                       "{ %0.2f | %0.2f } }\"]") %
                       nodeColoringAttribute % vertexId % map_[v].Base % map_[v].Reads %
                       map_[v].SpanningReads % map_[v].Score % map_[v].ReachingScore;
        }
    }

private:
    bool isInConsensus(PacBio::Poa::PoaGraph::Vertex v) const
    {
        return cssVtxs_.find(v) != cssVtxs_.end();
    }
    PacBio::Poa::detail::VertexInfoMap map_;
    std::set<PacBio::Poa::PoaGraph::Vertex> cssVtxs_;
    bool color_;
    bool verbose_;
};

class my_graph_writer
{
public:
    explicit my_graph_writer(bool leftToRight = false) : leftToRight_(leftToRight) {}
    void operator()(std::ostream& out) const
    {
        if (leftToRight_) {
            out << "rankdir=\"LR\";" << std::endl;
        }
    }

private:
    bool leftToRight_;
};

}  // namespace boost

namespace PacBio {
namespace Poa {
namespace detail {

// ----------------- PoaGraphImpl ---------------------

PoaGraphImpl::PoaGraphImpl()
    : g_()
    , vertexInfoMap_(get(boost::vertex_info, g_))
    , numReads_(0)
    , totalVertices_(0)
    , liveVertices_(0)
{
    enterVertex_ = addVertex('^', 0);
    exitVertex_ = addVertex('$', 0);
}

PoaGraphImpl::PoaGraphImpl(const PoaGraphImpl& other)
    : g_(other.g_)
    , vertexInfoMap_(get(boost::vertex_info, g_))
    , enterVertex_(other.enterVertex_)
    , exitVertex_(other.exitVertex_)
    , numReads_(other.numReads_)
{}

void PoaGraphImpl::repCheck() const
{
#ifndef NDEBUG
    // assert the representation invariant for the object
    BOOST_FOREACH (const VD v, vertices(g_)) {
        if (v == enterVertex_) {
            assert(in_degree(v, g_) == 0);
            assert(out_degree(v, g_) > 0 || NumReads() == 0);
        } else if (v == exitVertex_) {
            assert(in_degree(v, g_) > 0 || NumReads() == 0);
            assert(out_degree(v, g_) == 0);
        } else {
            assert(in_degree(v, g_) > 0);
            assert(out_degree(v, g_) > 0);
        }
    }
#endif
}

namespace {

std::vector<const AlignmentColumn*> getPredecessorColumns(const BoostGraph& g, VD v,
                                                          const AlignmentColumnMap& colMap)
{
    std::vector<const AlignmentColumn*> predecessorColumns;
    for (auto e : inEdges(v, g)) {
        VD u = source(e, g);
        const AlignmentColumn* const predCol = colMap.at(u).get();
        assert(predCol != nullptr);
        predecessorColumns.push_back(predCol);
    }
    return predecessorColumns;
}

}  // namespace

std::unique_ptr<PoaConsensus> PoaGraphImpl::FindConsensus(const Align::AlignConfig& config,
                                                          int minCoverage)
{
    std::vector<VD> bestPath = consensusPath(config.Mode, minCoverage);
    std::string consensusSequence = sequenceAlongPath(g_, vertexInfoMap_, bestPath);
    return std::make_unique<PoaConsensus>(consensusSequence, *this, externalizePath(bestPath));
}

std::unique_ptr<const AlignmentColumn> PoaGraphImpl::makeAlignmentColumnForExit(
    VD v, const AlignmentColumnMap& colMap, const std::string& sequence,
    const Align::AlignConfig& config) const
{
    assert(out_degree(v, g_) == 0);

    // this is kind of unnecessary as we are only actually using one entry in
    // this column
    int I = sequence.length();

    float bestScore = -FLT_MAX;
    VD prevVertex = null_vertex;

    // Under local or semiglobal alignment the vertex $ can be
    // "reached" in the dynamic programming from any other vertex
    // in one step via the End move--not just its predecessors in
    // the graph.  In local alignment, it may have been from any
    // row, not necessarily I.
    if (config.Mode == Align::AlignMode::SEMIGLOBAL || config.Mode == Align::AlignMode::LOCAL) {
        BOOST_FOREACH (const VD u, vertices(g_)) {
            if (u != exitVertex_) {
                const AlignmentColumn* const predCol = colMap.at(u).get();
                int prevRow = (config.Mode == Align::AlignMode::LOCAL ? ArgMax(predCol->Score) : I);
                if (predCol->HasRow(prevRow) && predCol->Score[prevRow] > bestScore) {
                    bestScore = predCol->Score[prevRow];
                    prevVertex = predCol->CurrentVertex;
                }
            }
        }
    } else {
        // regular predecessors
        std::vector<const AlignmentColumn*> predecessorColumns =
            getPredecessorColumns(g_, v, colMap);
        for (const AlignmentColumn* predCol : predecessorColumns) {
            if (predCol->HasRow(I) && predCol->Score[I] > bestScore) {
                bestScore = predCol->Score[I];
                prevVertex = predCol->CurrentVertex;
            }
        }
    }

    assert(prevVertex != null_vertex);
    AlignmentColumn curCol{v, 0, I + 1};
    curCol.Score[I] = bestScore;
    curCol.PreviousVertex[I] = prevVertex;
    curCol.ReachingMove[I] = EndMove;
    return std::make_unique<const AlignmentColumn>(std::move(curCol));
}

std::unique_ptr<const AlignmentColumn> PoaGraphImpl::makeAlignmentColumn(
    VD v, const AlignmentColumnMap& colMap, const std::string& sequence,
    const Align::AlignConfig& config, int beginRow, int endRow) const
{
    if (beginRow > endRow) {
        // This happens when there are no anchors in the read.  We
        // want this read to be threaded onto the graph as a singleton
        // path.  We use the START move.

        // This is only going to work in LOCAL aln, assert on that

        AlignmentColumn col{v, 0, 1};
        col.ReachingMove[0] = StartMove;
        col.PreviousVertex[0] = enterVertex_;
        col.Score[0] = 0;  // > -FLT_MAX;
        return std::make_unique<const AlignmentColumn>(std::move(col));
    }

    assert(beginRow < endRow || beginRow == 0 || beginRow == static_cast<int>(sequence.length()));

    AlignmentColumn curCol{v, beginRow, endRow};

    const PoaNode& vertexInfo = vertexInfoMap_[v];
    std::vector<const AlignmentColumn*> predecessorColumns = getPredecessorColumns(g_, v, colMap);

    // i represents position in array
    // readPos=i-1 represents position in read
    for (int i = beginRow; i < endRow; i++) {
        assert(curCol.HasRow(i));

        float candidateScore;
        float bestScore;
        VD prevVertex;
        MoveType reachingMove;

        if (config.Mode == Align::AlignMode::LOCAL) {
            bestScore = 0;
            prevVertex = enterVertex_;
            reachingMove = StartMove;
        } else {
            bestScore = -FLT_MAX;
            prevVertex = null_vertex;
            reachingMove = InvalidMove;
        }

        // Special-case the first row, this could probably be factored
        // more cleanly
        if (i == 0) {
            if (predecessorColumns.size() == 0) {
                // if this vertex doesn't have any in-edges it is ^; has
                // no reaching move
                assert(v == enterVertex_);
                curCol.Score[0] = 0;
                curCol.ReachingMove[0] = InvalidMove;
                curCol.PreviousVertex[0] = null_vertex;
            } else if (config.Mode == Align::AlignMode::SEMIGLOBAL ||
                       config.Mode == Align::AlignMode::LOCAL) {
                // under semiglobal or local alignment, we use the Start move
                curCol.Score[0] = 0;
                curCol.ReachingMove[0] = StartMove;
                curCol.PreviousVertex[0] = enterVertex_;
            } else {
                // otherwise it's a deletion
                for (const AlignmentColumn* prevCol : predecessorColumns) {
                    candidateScore = prevCol->Score[0] + config.Params.Delete;
                    if (candidateScore > bestScore) {
                        bestScore = candidateScore;
                        prevVertex = prevCol->CurrentVertex;
                        reachingMove = DeleteMove;
                    }
                }
                assert(reachingMove != InvalidMove);
                curCol.Score[0] = bestScore;
                curCol.ReachingMove[0] = reachingMove;
                curCol.PreviousVertex[0] = prevVertex;
            }
        } else {
            for (const AlignmentColumn* prevCol : predecessorColumns) {
                // Incorporate (Match or Mismatch)
                if (prevCol->HasRow(i - 1)) {
                    bool isMatch = sequence[i - 1] == vertexInfo.Base;
                    candidateScore = prevCol->Score[i - 1] +
                                     (isMatch ? config.Params.Match : config.Params.Mismatch);
                    if (candidateScore > bestScore) {
                        bestScore = candidateScore;
                        prevVertex = prevCol->CurrentVertex;
                        reachingMove = (isMatch ? MatchMove : MismatchMove);
                    }
                }
                // Delete
                if (prevCol->HasRow(i)) {
                    candidateScore = prevCol->Score[i] + config.Params.Delete;
                    if (candidateScore > bestScore) {
                        bestScore = candidateScore;
                        prevVertex = prevCol->CurrentVertex;
                        reachingMove = DeleteMove;
                    }
                }
            }
            // Extra
            if (curCol.HasRow(i - 1)) {
                candidateScore = curCol.Score[i - 1] + config.Params.Insert;
                if (candidateScore > bestScore) {
                    bestScore = candidateScore;
                    prevVertex = v;
                    reachingMove = ExtraMove;
                }
            }
            assert(reachingMove != InvalidMove);
            curCol.Score[i] = bestScore;
            curCol.ReachingMove[i] = reachingMove;
            curCol.PreviousVertex[i] = prevVertex;
        }
    }

    return std::make_unique<const AlignmentColumn>(std::move(curCol));
}

void PoaGraphImpl::AddRead(const std::string& readSeq, const Align::AlignConfig& config,
                           SdpRangeFinder* rangeFinder, std::vector<Vertex>* readPathOutput)
{
    if (NumReads() == 0) {
        AddFirstRead(readSeq, readPathOutput);
    } else {
        std::unique_ptr<PoaAlignmentMatrix> mat{TryAddRead(readSeq, config, rangeFinder)};
        CommitAdd(mat.get(), readPathOutput);
    }
}

void PoaGraphImpl::AddFirstRead(const std::string& readSeq, std::vector<Vertex>* readPathOutput)
{
    repCheck();
    assert(readSeq.length() > 0);
    assert(numReads_ == 0);

    threadFirstRead(readSeq, readPathOutput);
    numReads_++;
    repCheck();
}

std::unique_ptr<PoaAlignmentMatrix> PoaGraphImpl::TryAddRead(
    const std::string& readSeq, const Align::AlignConfig& config,
    SdpRangeFinder* const rangeFinder) const
{
    repCheck();
    assert(readSeq.length() > 0);
    assert(numReads_ > 0);

    // Prepare the range finder, if applicable
    if (rangeFinder != nullptr) {
        // NB: no minCoverage applicable here; this
        // "intermediate" consensus may include extra sequence
        // at either end
        std::vector<VD> cssPath = consensusPath(config.Mode);
        std::string cssSeq = sequenceAlongPath(g_, vertexInfoMap_, cssPath);
        rangeFinder->InitRangeFinder(*this, externalizePath(cssPath), cssSeq, readSeq);
    }

    // Calculate alignment columns of sequence vs. graph, using sparsity if
    // we have a range finder.
    auto mat = std::make_unique<PoaAlignmentMatrixImpl>();
    mat->readSequence_ = readSeq;
    mat->mode_ = config.Mode;
    mat->graph_ = this;

    std::vector<VD> sortedVerticesLocal(num_vertices(g_));
    topological_sort(g_, sortedVerticesLocal.rbegin());
    std::unique_ptr<const AlignmentColumn> curCol;
    for (const auto& v : sortedVerticesLocal) {
        if (v != exitVertex_) {
            size_t startRow = 0;
            size_t endRow = readSeq.size() + 1;
            if (rangeFinder) {
                // FindAlignableRange returns an alignable sequence range, which is not the same as
                // the alignable rows (the end is off-by-one, for a normal interval).
                int startRange;
                int endRange;
                std::tie(startRange, endRange) = rangeFinder->FindAlignableRange(externalize(v));
                startRow = startRange;
                endRow = (endRange == -INT_MAX / 2 ? endRange : endRange + 1);
            }
            /* Vertex vExt = */ externalize(v);  // DEBUGGING
            curCol = makeAlignmentColumn(v, mat->columns_, readSeq, config, startRow, endRow);
        } else {
            curCol = makeAlignmentColumnForExit(v, mat->columns_, readSeq, config);
        }
        mat->columns_[v] = std::move(curCol);
    }

    mat->score_ = mat->columns_[exitVertex_]->Score[readSeq.size()];
    repCheck();

    PoaAlignmentMatrix* base = mat.release();
    std::unique_ptr<PoaAlignmentMatrix> result;
    result.reset(base);
    return result;
}

void PoaGraphImpl::CommitAdd(const PoaAlignmentMatrix* const mat_,
                             std::vector<Vertex>* readPathOutput)
{
    repCheck();

    const auto* mat = static_cast<const PoaAlignmentMatrixImpl*>(mat_);
    tracebackAndThread(mat->readSequence_, mat->columns_, mat->mode_, readPathOutput);
    numReads_++;

    repCheck();
}

class VertexComparator
{
public:
    explicit VertexComparator(const BoostGraph& g) : g_(g) {}

    // Use existing indices to provide a stable memory-agnostic ordering
    bool operator()(VD v1, VD v2) const
    {
        const int vi1 = get(boost::vertex_index, g_, v1);
        const int vi2 = get(boost::vertex_index, g_, v2);
        return (vi1 < vi2);
    }

private:
    const BoostGraph& g_;
};

void PoaGraphImpl::PruneGraph(const int minCoverage)
{
    // We have to use an iterator since we're modifying the graph as we go
    boost::graph_traits<BoostGraph>::vertex_iterator vi, vi_end, next;
    std::tie(vi, vi_end) = vertices(g_);
    for (next = vi; vi != vi_end; vi = next) {
        ++next;
        if (vertexInfoMap_[*vi].Reads < minCoverage) {
            clear_vertex(*vi, g_);
            remove_vertex(*vi, g_);
        }
    }

    // This is a workaround for the non-deterministic order of iteration
    // from BGL's vertices, which like edges also appear to be ordered
    // by memory address (see: http://stackoverflow.com/questions/30968690/)

    // We can't use topological sort here, since having a vertex index greater
    // than num_vertices(g_) causes a crash. Instead we sort the vertices by
    // their existing index.  This should still be very fast, as the underlying
    // collection should be mostly sorted already.
    std::vector<VD> sortedVerticesLocal;
    BGL_FORALL_VERTICES(v, g_, BoostGraph) sortedVerticesLocal.push_back(v);
    std::sort(sortedVerticesLocal.begin(), sortedVerticesLocal.end(), VertexComparator(g_));

    // Finally, re-index all vertices to the range [0, num_vertices(g_)]
    boost::graph_traits<BoostGraph>::vertices_size_type current_index = 0;
    index_map_t index_map = get(boost::vertex_index, g_);
    for (const VD& v : sortedVerticesLocal) {
        index_map[v] = current_index++;
    }
}

size_t PoaGraphImpl::NumReads() const { return numReads_; }

std::string PoaGraphImpl::ToGraphViz(int flags, const PoaConsensus* pc) const
{
    std::ostringstream ss;
    write_graphviz(ss, g_,
                   boost::my_label_writer(vertexInfoMap_, flags & PoaGraph::COLOR_NODES,
                                          flags & PoaGraph::VERBOSE_NODES, pc),
                   boost::default_writer(),  // edge writer
                   boost::my_graph_writer(true));

    return ss.str();
}

void PoaGraphImpl::WriteGraphVizFile(const std::filesystem::path& filename, int flags,
                                     const PoaConsensus* pc) const
{
    std::ofstream outfile{filename};
    outfile << ToGraphViz(flags, pc);
}

void PoaGraphImpl::WriteGraphCsvFile(const std::filesystem::path& filename) const
{
    std::ofstream outfile{filename};

    std::list<VD> sortedVerticesLocal(num_vertices(g_));
    topological_sort(g_, sortedVerticesLocal.rbegin());

    outfile << "Id,Base,Reads,SpanningReads,Score,ReachingScore" << std::endl;
    for (const VD v : sortedVerticesLocal) {
        PoaNode& vi = vertexInfoMap_[v];
        outfile << vi.Id << ',' << vi.Base << ',' << vi.Reads << ',' << vi.SpanningReads << ','
                << vi.Score << ',' << vi.ReachingScore << std::endl;
    }
}

}  // namespace detail
}  // namespace Poa
}  // namespace PacBio
