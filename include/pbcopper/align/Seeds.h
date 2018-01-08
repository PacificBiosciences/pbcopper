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

#ifndef PBCOPPER_ALIGN_SEEDS_H
#define PBCOPPER_ALIGN_SEEDS_H

#include <functional>
#include <set>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/align/Seed.h>

namespace PacBio {
namespace Align {

///
/// \brief The Seeds class represents a container of Seed objects.
///
class Seeds
{
public:
    Seeds(void);

    Seeds(const Seeds&) = default;
    Seeds(Seeds&&) = default;
    Seeds& operator=(const Seeds&) = default;
    Seeds& operator=(Seeds&&) = default;
    ~Seeds(void) = default;

public:
    ///
    /// \brief AddSeed
    ///
    /// Inserts seed into container.
    ///
    /// \param s
    /// \return
    ///
    bool AddSeed(const Seed& s);

    ///
    /// \brief TryMergeSeed
    ///
    /// Attempts to merge seed \p s with a suitable seed in the container. Seeds
    /// must overlap to be merge-able.
    ///
    /// \param s
    /// \return true if seed was merged into existing seed or not, false if no
    ///         suitable candidate could be found
    ///
    bool TryMergeSeed(const Seed& s);

public:
    // STL compatibility
    using container_t     = std::multiset<Seed>;

    using iterator        = container_t::iterator;
    using const_iterator  = container_t::const_iterator;
    using reference       = container_t::reference;
    using const_reference = container_t::const_reference;
    using size_type       = container_t::size_type;

    iterator       begin(void)        { return data_.begin(); }
    const_iterator begin(void) const  { return data_.begin(); }
    const_iterator cbegin(void) const { return data_.cbegin(); }

    iterator       end(void)          { return data_.end(); }
    const_iterator end(void) const    { return data_.end(); }
    const_iterator cend(void) const   { return data_.cend(); }

    const_reference back(void) const  { return *data_.crbegin(); }
    const_reference front(void) const { return *data_.cbegin(); }

    bool      empty(void) const { return data_.empty(); }
    size_type size(void) const  { return data_.size(); }

private:
    container_t data_;
};

} // namespace Align
} // namespace PacBio

#include <pbcopper/align/internal/Seeds-inl.h>

#endif // PBCOPPER_ALIGN_SEEDS_H
