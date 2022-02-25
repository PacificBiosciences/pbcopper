// The POA "alignment matrix" is a set of alignment columns
// corresponding to each vertex in the graph that the read was aligned
// against.

#ifndef PBCOPPER_POA_ALIGNMENTMATRIX_H
#define PBCOPPER_POA_ALIGNMENTMATRIX_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/poa/PoaGraph.h>
#include "BoostGraph.h"
#include "VectorL.h"

#include <boost/utility.hpp>

#include <memory>

#include <cfloat>
#include <cstddef>

namespace PacBio {
namespace Poa {
namespace detail {

enum MoveType
{
    InvalidMove,  // Invalid move reaching ^ (start)
    StartMove,    // Start move: ^ -> vertex in row 0 of local alignment
    EndMove,      // End move: vertex -> $ in row 0 of local alignment, or
                  //  in global alignment, terminal vertex -> $
    MatchMove,
    MismatchMove,
    DeleteMove,
    ExtraMove
};

class AlignmentColumn : boost::noncopyable
{
public:
    VD CurrentVertex;
    VectorL<float> Score;
    VectorL<MoveType> ReachingMove;
    VectorL<VD> PreviousVertex;

    explicit AlignmentColumn(VD vertex, int beginRow, int endRow)
        : CurrentVertex(vertex)
        , Score(beginRow, endRow, -FLT_MAX)
        , ReachingMove(beginRow, endRow, InvalidMove)
        , PreviousVertex(beginRow, endRow, null_vertex)
    {}

    AlignmentColumn(AlignmentColumn&& other) noexcept
        : CurrentVertex(std::move(other.CurrentVertex))
        , Score(std::move(other.Score))
        , ReachingMove(std::move(other.ReachingMove))
        , PreviousVertex(std::move(other.PreviousVertex))
    {}

    AlignmentColumn& operator=(AlignmentColumn&& other) noexcept
    {
        CurrentVertex = std::move(other.CurrentVertex);
        Score = std::move(other.Score);
        ReachingMove = std::move(other.ReachingMove);
        PreviousVertex = std::move(other.PreviousVertex);
        return *this;
    }

    size_t BeginRow() const { return Score.BeginRow(); }
    size_t EndRow() const { return Score.EndRow(); }
    bool HasRow(size_t i) const { return (BeginRow() <= i) && (i < EndRow()); }
};

using AlignmentColumnMap = std::unordered_map<VD, std::unique_ptr<const AlignmentColumn>>;

class PoaAlignmentMatrixImpl : public PoaAlignmentMatrix
{
public:
    virtual ~PoaAlignmentMatrixImpl() = default;

    virtual float Score() const { return score_; }
    size_t NumRows() const { return readSequence_.length() + 1; }
    size_t NumCols() const { return columns_.size(); }
    void Print() const;

    // TODO: why did I leave these public?  why is there no
    // constructor?
    const PoaGraphImpl* graph_;
    AlignmentColumnMap columns_;
    std::string readSequence_;
    Align::AlignMode mode_;
    float score_;
};
}  // namespace detail
}  // namespace Poa
}  // namespace PacBio
#endif  // PBCOPPER_POA_ALIGNMENTMATRIX_H
