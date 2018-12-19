// Author: Derek Barnett

#ifndef PBCOPPER_ALIGN_SEED_INL_H
#define PBCOPPER_ALIGN_SEED_INL_H

#include <algorithm>
#include <cassert>
#include <cmath>

#include <pbcopper/align/Seed.h>

namespace PacBio {
namespace Align {

inline Seed::Seed()
    : beginPositionH_{0}
    , beginPositionV_{0}
    , endPositionH_{0}
    , endPositionV_{0}
    , lowerDiagonal_{0}
    , upperDiagonal_{0}
    , score_{0}
{
}

inline Seed::Seed(const uint64_t beginPosH, const uint64_t beginPosV, const uint64_t seedLength)
    : beginPositionH_{beginPosH}
    , beginPositionV_{beginPosV}
    , endPositionH_{beginPosH + seedLength}
    , endPositionV_{beginPosV + seedLength}
    , lowerDiagonal_{static_cast<int64_t>(beginPosH - beginPosV)}
    , upperDiagonal_{static_cast<int64_t>(beginPosH - beginPosV)}
    , score_{0}
{
}

inline Seed::Seed(const uint64_t beginPosH, const uint64_t beginPosV, const uint64_t endPosH,
                  const uint64_t endPosV)
    : beginPositionH_{beginPosH}
    , beginPositionV_{beginPosV}
    , endPositionH_{endPosH}
    , endPositionV_{endPosV}
    , lowerDiagonal_{std::min(static_cast<int64_t>(beginPosH - beginPosV),
                              static_cast<int64_t>(endPosH - endPosV))}
    , upperDiagonal_{std::max(static_cast<int64_t>(beginPosH - beginPosV),
                              static_cast<int64_t>(endPosH - endPosV))}
    , score_{0}
{
    assert(upperDiagonal_ >= lowerDiagonal_);
}

inline int64_t Seed::BeginDiagonal() const
{
    return static_cast<int64_t>(beginPositionH_ - beginPositionV_);
}

inline uint64_t Seed::BeginPositionH() const { return beginPositionH_; }

inline Seed& Seed::BeginPositionH(const uint64_t beginPosH)
{
    beginPositionH_ = beginPosH;
    return *this;
}

inline uint64_t Seed::BeginPositionV() const { return beginPositionV_; }

inline Seed& Seed::BeginPositionV(const uint64_t beginPosV)
{
    beginPositionV_ = beginPosV;
    return *this;
}

inline int64_t Seed::Diagonal() const
{
    if (beginPositionV_ > beginPositionH_)
        return -static_cast<int64_t>(beginPositionV_ - beginPositionH_);
    else
        return static_cast<int64_t>(beginPositionH_ - beginPositionV_);
}

inline int64_t Seed::EndDiagonal() const
{
    return static_cast<int64_t>(endPositionH_ - endPositionV_);
}

inline uint64_t Seed::EndPositionH() const { return endPositionH_; }

inline Seed& Seed::EndPositionH(const uint64_t endPosH)
{
    endPositionH_ = endPosH;
    return *this;
}

inline uint64_t Seed::EndPositionV() const { return endPositionV_; }

inline Seed& Seed::EndPositionV(const uint64_t endPosV)
{
    endPositionV_ = endPosV;
    return *this;
}

inline int64_t Seed::LowerDiagonal() const { return lowerDiagonal_; }

inline Seed& Seed::LowerDiagonal(const int64_t lowerDiagonal)
{
    lowerDiagonal_ = lowerDiagonal;
    return *this;
}

inline int64_t Seed::UpperDiagonal() const { return upperDiagonal_; }

inline Seed& Seed::UpperDiagonal(const int64_t upperDiagonal)
{
    upperDiagonal_ = upperDiagonal;
    return *this;
}

inline int32_t Seed::Score() const { return score_; }

inline Seed& Seed::Score(const int32_t score)
{
    score_ = score;
    return *this;
}

inline uint64_t Seed::Size() const
{
    return std::max(endPositionH_ - beginPositionH_, endPositionV_ - beginPositionV_);
}

inline Seed& Seed::operator+=(const Seed& other)
{
    // cached initial, input sizes
    const auto initialSize = Size();
    const auto otherSize = other.Size();

    // calculate new coordinates
    beginPositionH_ = std::min(beginPositionH_, other.beginPositionH_);
    beginPositionV_ = std::min(beginPositionV_, other.beginPositionV_);
    endPositionH_ = std::max(endPositionH_, other.endPositionH_);
    endPositionV_ = std::max(endPositionV_, other.endPositionV_);
    lowerDiagonal_ = std::min(lowerDiagonal_, other.lowerDiagonal_);
    upperDiagonal_ = std::max(upperDiagonal_, other.upperDiagonal_);

    // get new size & overlap length
    const auto newSize = Size();
    const auto overlap = initialSize + otherSize - newSize;

    // sanity checks:
    //  - new size must be >= each input seed
    //  - new size must be <= total input seed lengths
    //  - overlap must be <= each input seed
    //
    assert(newSize >= initialSize);
    assert(newSize >= otherSize);
    assert(newSize <= (initialSize + otherSize));
    assert(overlap <= otherSize);
    assert(overlap <= initialSize);

    // adjust score depending on fraction used from each seed
    const double total = initialSize + otherSize - overlap;
    const double intoFraction = (initialSize - 0.5 * overlap) / total;
    const double fromFraction = (otherSize - 0.5 * overlap) / total;
    const auto newScore =
        static_cast<int32_t>(std::round(intoFraction * Score() + fromFraction * other.Score()));
    score_ = newScore;
    return *this;
}

inline Seed Seed::operator+(const Seed& other) const
{
    Seed s(*this);
    s += other;
    return s;
}

inline bool operator<(const Seed& lhs, const Seed& rhs)
{
    return lhs.BeginDiagonal() < rhs.BeginDiagonal();
}

inline bool operator==(const Seed& lhs, const Seed& rhs)
{
    return lhs.BeginPositionH() == rhs.BeginPositionH() &&
           lhs.BeginPositionV() == rhs.BeginPositionV() &&
           lhs.EndPositionH() == rhs.EndPositionH() && lhs.EndPositionV() == rhs.EndPositionV() &&
           lhs.LowerDiagonal() == rhs.LowerDiagonal() &&
           lhs.UpperDiagonal() == rhs.UpperDiagonal() && lhs.Score() == rhs.Score();
}

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_SEED_INL_H
