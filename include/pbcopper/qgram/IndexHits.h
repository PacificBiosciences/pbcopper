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

#ifndef PBCOPPER_QGRAM_INDEXHITS_H
#define PBCOPPER_QGRAM_INDEXHITS_H

#include <cassert>
#include <cstddef>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/qgram/IndexHit.h>

namespace PacBio {
namespace QGram {

///
/// \brief The IndexHits class provides a "view" onto a contiguous run of
///        IndexHit objects for a given q-gram.
///
/// \note An instance of this class must not outlive the Index that generated it.
///
class IndexHits
{
public:
    ///
    /// \brief IndexHits
    /// \param source
    /// \param seqPos
    ///
    IndexHits(const std::vector<IndexHit>* source, const size_t queryPos)
        : IndexHits(source, 0, source->size(), queryPos)
    {
    }

    ///
    /// \brief IndexHits
    /// \param source
    /// \param begin
    /// \param end
    ///
    IndexHits(const std::vector<IndexHit>* source, const size_t begin, const size_t end,
              const size_t queryPos)
        : source_(source), begin_(begin), end_(end), queryPos_(queryPos)
    {
        assert(source);
        assert(begin <= end);
    }

    IndexHits(const IndexHits&) = default;
    IndexHits(IndexHits&&) = default;
    ~IndexHits(void) = default;

public:
    size_t QueryPosition(void) const { return queryPos_; }

public:
    /// \name STL compatibility
    /// \{

    using iterator = std::vector<IndexHit>::iterator;
    using const_iterator = std::vector<IndexHit>::const_iterator;
    using reference = std::vector<IndexHit>::reference;
    using const_reference = std::vector<IndexHit>::const_reference;
    using size_type = std::vector<IndexHit>::size_type;

    const_iterator begin(void) const { return source_->begin() + begin_; }
    const_iterator cbegin(void) const { return source_->cbegin() + begin_; }
    const_iterator end(void) const { return begin() + size(); }
    const_iterator cend(void) const { return cbegin() + size(); }
    size_type size(void) const { return end_ - begin_; }

    const_reference operator[](size_type pos) const { return *(source_->begin() + begin_ + pos); }

    const_reference at(size_type pos) const
    {
        assert(pos < size());
        return *(source_->begin() + begin_ + pos);
    }

    /// \}

private:
    const std::vector<IndexHit>* source_;
    const size_t begin_;
    const size_t end_;
    const size_t queryPos_;
};

}  // namespace QGram
}  // namespace PacBio

#endif  // PBCOPPER_QGRAM_INDEXHITS_H
