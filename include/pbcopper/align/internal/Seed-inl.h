// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Derek Barnett

#ifndef PBCOPPER_ALIGN_SEED_INL_H
#define PBCOPPER_ALIGN_SEED_INL_H

#include <cassert>
#include <cmath>
#include <algorithm>

#include <pbcopper/align/Seed.h>

namespace PacBio {
namespace Align {

inline Seed::Seed(void)
    : beginPositionH_{0}
    , beginPositionV_{0}
    , endPositionH_{0}
    , endPositionV_{0}
    , lowerDiagonal_{0}
    , upperDiagonal_{0}
    , score_{0}
{ }

inline Seed::Seed(const uint64_t beginPosH,
                  const uint64_t beginPosV,
                  const uint64_t seedLength)
    : beginPositionH_{beginPosH}
    , beginPositionV_{beginPosV}
    , endPositionH_{beginPosH + seedLength}
    , endPositionV_{beginPosV + seedLength}
    , lowerDiagonal_{static_cast<int64_t>(beginPosH - beginPosV)}
    , upperDiagonal_{static_cast<int64_t>(beginPosH - beginPosV)}
    , score_{0}
{  }

inline Seed::Seed(const uint64_t beginPosH,
                  const uint64_t beginPosV,
                  const uint64_t endPosH,
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

inline int64_t Seed::BeginDiagonal(void) const
{ return static_cast<int64_t>(beginPositionH_ - beginPositionV_); }

inline uint64_t Seed::BeginPositionH(void) const
{ return beginPositionH_; }

inline Seed& Seed::BeginPositionH(const uint64_t beginPosH)
{ beginPositionH_ = beginPosH; return *this; }

inline uint64_t Seed::BeginPositionV(void) const
{ return beginPositionV_; }

inline Seed& Seed::BeginPositionV(const uint64_t beginPosV)
{ beginPositionV_ = beginPosV; return *this; }

inline int64_t Seed::Diagonal(void) const
{
    if (beginPositionV_ > beginPositionH_)
        return -static_cast<int64_t>(beginPositionV_ - beginPositionH_) ;
    else
        return static_cast<int64_t>(beginPositionH_ - beginPositionV_);
}

inline int64_t Seed::EndDiagonal(void) const
{ return static_cast<int64_t>(endPositionH_ - endPositionV_); }

inline uint64_t Seed::EndPositionH(void) const
{ return endPositionH_; }

inline Seed& Seed::EndPositionH(const uint64_t endPosH)
{ endPositionH_ = endPosH; return *this; }

inline uint64_t Seed::EndPositionV(void) const
{ return endPositionV_; }

inline Seed& Seed::EndPositionV(const uint64_t endPosV)
{ endPositionV_ = endPosV; return *this; }

inline int64_t Seed::LowerDiagonal(void) const
{ return lowerDiagonal_; }

inline Seed& Seed::LowerDiagonal(const int64_t lowerDiagonal)
{ lowerDiagonal_ = lowerDiagonal; return *this; }

inline int64_t Seed::UpperDiagonal(void) const
{ return upperDiagonal_; }

inline Seed& Seed::UpperDiagonal(const int64_t upperDiagonal)
{ upperDiagonal_ = upperDiagonal; return *this; }

inline int32_t Seed::Score(void) const
{ return score_; }

inline Seed& Seed::Score(const int32_t score)
{ score_ = score; return *this; }

inline uint64_t Seed::Size(void) const
{
    return std::max(endPositionH_ - beginPositionH_,
                    endPositionV_ - beginPositionV_);
}

inline Seed& Seed::operator+=(const Seed& other)
{
    // cached initial, input sizes
    const auto initialSize = Size();
    const auto otherSize = other.Size();

    // calculate new coordinates
    beginPositionH_ = std::min(beginPositionH_, other.beginPositionH_);
    beginPositionV_ = std::min(beginPositionV_, other.beginPositionV_);
    endPositionH_   = std::max(endPositionH_,   other.endPositionH_);
    endPositionV_   = std::max(endPositionV_,   other.endPositionV_);
    lowerDiagonal_  = std::min(lowerDiagonal_,  other.lowerDiagonal_);
    upperDiagonal_  = std::max(upperDiagonal_,  other.upperDiagonal_);

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
    const auto total = static_cast<double>(initialSize + otherSize - overlap);
    const auto intoFraction = static_cast<double>(initialSize - 0.5*overlap) / total;
    const auto fromFraction = static_cast<double>(otherSize - 0.5*overlap) / total;
    const auto newScore = static_cast<int32_t>(std::round(intoFraction*Score() +
                                                          fromFraction*other.Score()));
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
{ return lhs.BeginDiagonal() < rhs.BeginDiagonal(); }

inline bool operator==(const Seed& lhs, const Seed& rhs)
{
    return lhs.BeginPositionH() == rhs.BeginPositionH() &&
           lhs.BeginPositionV() == rhs.BeginPositionV() &&
           lhs.EndPositionH()   == rhs.EndPositionH()   &&
           lhs.EndPositionV()   == rhs.EndPositionV()   &&
           lhs.LowerDiagonal()  == rhs.LowerDiagonal()  &&
           lhs.UpperDiagonal()  == rhs.UpperDiagonal()  &&
           lhs.Score()          == rhs.Score();
}

} // namespace Align
} // namespace PacBio

#endif // PBCOPPER_ALIGN_SEED_INL_H
