#include <pbcopper/align/Seed.h>

#include <cassert>
#include <cmath>

#include <algorithm>
#include <tuple>
#include <type_traits>

namespace PacBio {
namespace Align {

Seed::Seed(const std::uint64_t beginPosH, const std::uint64_t beginPosV,
           const std::uint64_t seedLength)
    : beginPositionH_{beginPosH}
    , beginPositionV_{beginPosV}
    , endPositionH_{beginPosH + seedLength}
    , endPositionV_{beginPosV + seedLength}
    , lowerDiagonal_{static_cast<std::int64_t>(beginPosH - beginPosV)}
    , upperDiagonal_{static_cast<std::int64_t>(beginPosH - beginPosV)}
    , score_{0}
{}

Seed::Seed(const std::uint64_t beginPosH, const std::uint64_t beginPosV,
           const std::uint64_t endPosH, const std::uint64_t endPosV)
    : beginPositionH_{beginPosH}
    , beginPositionV_{beginPosV}
    , endPositionH_{endPosH}
    , endPositionV_{endPosV}
    , lowerDiagonal_{std::min(static_cast<std::int64_t>(beginPosH - beginPosV),
                              static_cast<std::int64_t>(endPosH - endPosV))}
    , upperDiagonal_{std::max(static_cast<std::int64_t>(beginPosH - beginPosV),
                              static_cast<std::int64_t>(endPosH - endPosV))}
    , score_{0}
{
    assert(upperDiagonal_ >= lowerDiagonal_);
}

int64_t Seed::BeginDiagonal() const
{
    return static_cast<std::int64_t>(beginPositionH_ - beginPositionV_);
}

uint64_t Seed::BeginPositionH() const { return beginPositionH_; }

Seed& Seed::BeginPositionH(const std::uint64_t beginPosH)
{
    beginPositionH_ = beginPosH;
    return *this;
}

uint64_t Seed::BeginPositionV() const { return beginPositionV_; }

Seed& Seed::BeginPositionV(const std::uint64_t beginPosV)
{
    beginPositionV_ = beginPosV;
    return *this;
}

int64_t Seed::Diagonal() const
{
    if (beginPositionV_ > beginPositionH_) {
        return -static_cast<std::int64_t>(beginPositionV_ - beginPositionH_);
    } else {
        return static_cast<std::int64_t>(beginPositionH_ - beginPositionV_);
    }
}

int64_t Seed::EndDiagonal() const
{
    return static_cast<std::int64_t>(endPositionH_ - endPositionV_);
}

uint64_t Seed::EndPositionH() const { return endPositionH_; }

Seed& Seed::EndPositionH(const std::uint64_t endPosH)
{
    endPositionH_ = endPosH;
    return *this;
}

uint64_t Seed::EndPositionV() const { return endPositionV_; }

Seed& Seed::EndPositionV(const std::uint64_t endPosV)
{
    endPositionV_ = endPosV;
    return *this;
}

int64_t Seed::LowerDiagonal() const { return lowerDiagonal_; }

Seed& Seed::LowerDiagonal(const std::int64_t lowerDiagonal)
{
    lowerDiagonal_ = lowerDiagonal;
    return *this;
}

int64_t Seed::UpperDiagonal() const { return upperDiagonal_; }

Seed& Seed::UpperDiagonal(const std::int64_t upperDiagonal)
{
    upperDiagonal_ = upperDiagonal;
    return *this;
}

int32_t Seed::Score() const { return score_; }

Seed& Seed::Score(const std::int32_t score)
{
    score_ = score;
    return *this;
}

uint64_t Seed::Size() const
{
    return std::max(endPositionH_ - beginPositionH_, endPositionV_ - beginPositionV_);
}

Seed& Seed::operator+=(const Seed& other)
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
    const auto newScore = static_cast<std::int32_t>(
        std::round(intoFraction * Score() + fromFraction * other.Score()));
    score_ = newScore;
    return *this;
}

Seed Seed::operator+(const Seed& other) const
{
    Seed s(*this);
    s += other;
    return s;
}

bool operator<(const Seed& lhs, const Seed& rhs) noexcept
{
    return lhs.BeginDiagonal() < rhs.BeginDiagonal();
}

bool operator==(const Seed& lhs, const Seed& rhs) noexcept
{
    return std::make_tuple(lhs.BeginPositionH(), lhs.BeginPositionV(), lhs.EndPositionH(),
                           lhs.EndPositionV(), lhs.LowerDiagonal(), lhs.UpperDiagonal(),
                           lhs.Score()) == std::make_tuple(rhs.BeginPositionH(),
                                                           rhs.BeginPositionV(), rhs.EndPositionH(),
                                                           rhs.EndPositionV(), rhs.LowerDiagonal(),
                                                           rhs.UpperDiagonal(), rhs.Score());
}

}  // namespace Align
}  // namespace PacBio
