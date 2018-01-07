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

#ifndef PBCOPPER_QGRAM_INDEX_H
#define PBCOPPER_QGRAM_INDEX_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/qgram/IndexHits.h>

namespace PacBio {
namespace QGram {

namespace internal { class IndexImpl; }

///
/// \brief The Index class provides efficient retrieval of all occurrences of
///        a particular q-gram in the input sequence.
///
/// Algorithm and structure adapted to our needs from:
///     <seqan/index/index_qgram.h>
///
class Index
{
public:
    ///
    /// \brief Index
    /// \param[in] q    q-gram size
    /// \param[in] seq  construct index from this sequence
    /// \throws std::runtime_error if q-gram == 0
    ///
    Index(const size_t q, const std::string& seq);

    ///
    /// \brief Index
    /// \param[in] q    q-gram size
    /// \param[in] seq  construct index from this sequence
    /// \throws std::runtime_error if q-gram == 0
    ///
    Index(const size_t q, std::string&& seq);

    ///
    /// \brief Index
    /// \param[in] q        q-gram size
    /// \param[in] seqs     construct index from these sequences
    /// \throws std::runtime_error if q-gram == 0
    ///
    Index(const size_t q, const std::vector<std::string>& seqs);

    ///
    /// \brief Index
    /// \param[in] q        q-gram size
    /// \param[in] seqs     construct index from these sequences
    /// \throws std::runtime_error if q-gram == 0
    ///
    Index(const size_t q, std::vector<std::string>&& seqs);

public:

    ///
    /// \brief Hits
    /// \param[in] seq                  query sequence
    /// \param[in] filterHomopolymers   do not count hits on homopolymers (len == q)
    /// \return a vector of IndexHits results.
    ///
    std::vector<IndexHits> Hits(const std::string& seq,
                                const bool filterHomopolymers = false) const;

    ///
    /// \brief Size
    /// \return q-gram size
    ///
    size_t Size(void) const;

private:
    std::unique_ptr<internal::IndexImpl> d_;
};

} // namespace QGram
} // namespace PacBio

#include <pbcopper/qgram/internal/Index-inl.h>

#endif // PBCOPPER_QGRAM_INDEX_H
