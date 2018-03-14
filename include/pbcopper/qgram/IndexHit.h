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

#ifndef PBCOPPER_QGRAM_INDEXHIT_H
#define PBCOPPER_QGRAM_INDEXHIT_H

#include <cstdint>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace QGram {

///
/// \brief The IndexHit class represents a q-gram hit from a query onto input
///        sequence.
///
/// Each instance reports which sequence was hit (useful in the case of
/// multi-sequence indices) and at which position.
///
class IndexHit
{
public:
    ///
    /// \brief IndexHit
    ///
    IndexHit(void) : id_(0), position_(0) {}

    ///
    /// \brief IndexHit
    /// \param[in] id   index number of input sequence
    /// \param[in] pos  position in input sequence
    ///
    IndexHit(const uint32_t id, const uint64_t pos) : id_(id), position_(pos) {}

    IndexHit(const IndexHit&) = default;
    IndexHit(IndexHit&&) = default;
    IndexHit& operator=(const IndexHit&) = default;
    IndexHit& operator=(IndexHit&&) = default;

public:
    ///
    /// \brief Id
    /// \return id (index number) of Index input sequence containing this hit
    ///
    uint32_t Id(void) const { return id_; }

    ///
    /// \brief Position
    /// \return position of hit
    ///
    uint64_t Position(void) const { return position_; }

private:
    uint32_t id_;
    uint64_t position_;
};

///
/// \brief operator ==
/// \param[in] lhs
/// \param[in] rhs
/// \return true if the hits share the same sequence id & position
///
inline bool operator==(const IndexHit& lhs, const IndexHit& rhs)
{
    return lhs.Id() == rhs.Id() && lhs.Position() == rhs.Position();
}

}  // namespace QGram
}  // namespace PacBio

#endif  // PBCOPPER_QGRAM_INDEXHIT_H
