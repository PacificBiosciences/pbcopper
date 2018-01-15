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

#ifndef PBCOPPER_ALIGN_SEEDS_INL_H
#define PBCOPPER_ALIGN_SEEDS_INL_H

#include <pbcopper/align/Seeds.h>

namespace PacBio {
namespace Align {
namespace internal {

inline bool CanMergeSeeds(const Seed& lhs, const Seed& rhs)
{
    // rhs must be to right of lhs AND seeds must overlap
    const auto ok = rhs.BeginPositionH() >= lhs.BeginPositionH() &&
                    rhs.BeginPositionV() >= lhs.BeginPositionV() &&
                    rhs.BeginPositionH() <= lhs.EndPositionH() &&
                    rhs.BeginPositionV() <= lhs.EndPositionV();
    return ok;
}

}  // namespace internal

inline Seeds::Seeds(void) {}

inline bool Seeds::AddSeed(const Seed& s)
{
    data_.insert(s);
    return true;
}

inline bool Seeds::TryMergeSeed(const Seed& s)
{
    Seed left;
    for (auto it = data_.begin(); it != data_.end(); ++it) {
        if (internal::CanMergeSeeds(*it, s)) {
            left = *it;
            left += s;
            data_.erase(it);
            data_.insert(left);
            return true;
        } else if (internal::CanMergeSeeds(s, *it)) {
            left = s;
            left += *it;
            data_.erase(it);
            data_.insert(left);
            return true;
        }
    }
    return false;
}

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_SEEDS_INL_H
