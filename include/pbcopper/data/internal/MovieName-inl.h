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

#ifndef PBCOPPER_DATA_MOVIENAME_INL_H
#define PBCOPPER_DATA_MOVIENAME_INL_H

#include <cassert>
#include <iostream>
#include <stdexcept>

#include <pbcopper/data/MovieName.h>
#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace Data {

struct MovieName::PartsCache
{
    boost::string_ref instrumentName_;
    boost::string_ref runStartTime_;
};

// NOTE: We're not going to re-calculate cache in copies until actually needed.
//       We can't completely re-use the original cache, since the string_refs
//       point to other string::c_str(). So we won't pay for the calculation
//       until the components are again requested. That should happen less often
//       than moving these guys around.

inline MovieName::MovieName(void) : partsCache_(nullptr) {}

inline MovieName::MovieName(const MovieName& other)
    : movieName_(other.movieName_), partsCache_(nullptr)
{
}

inline MovieName::MovieName(MovieName&& other) noexcept
    : movieName_(std::move(other.movieName_)), partsCache_(std::move(other.partsCache_))
{
}

inline MovieName& MovieName::operator=(const MovieName& other)
{
    movieName_ = other.movieName_;
    partsCache_.reset(nullptr);
    return *this;
}

inline MovieName& MovieName::operator=(MovieName&& other) noexcept
{
    movieName_ = std::move(other.movieName_);
    partsCache_ = std::move(other.partsCache_);
    return *this;
}

inline MovieName::MovieName(const std::string& name) : movieName_(name), partsCache_(nullptr) {}

inline MovieName::MovieName(std::string&& name) : movieName_(std::move(name)), partsCache_(nullptr)
{
}

inline MovieName::~MovieName(void) {}

inline boost::string_ref MovieName::InstrumentName(void) const
{
    if (!partsCache_) UpdatePartsCache();
    assert(partsCache_);
    return partsCache_->instrumentName_;
}

inline boost::string_ref MovieName::RunStartTime(void) const
{
    if (!partsCache_) UpdatePartsCache();
    assert(partsCache_);
    return partsCache_->runStartTime_;
}

inline std::string MovieName::ToStdString(void) const { return movieName_; }

inline bool operator==(const MovieName& lhs, const MovieName& rhs)
{
    return lhs.ToStdString() == rhs.ToStdString();
}

inline bool operator!=(const MovieName& lhs, const MovieName& rhs) { return !(lhs == rhs); }

inline bool operator<(const MovieName& lhs, const MovieName& rhs)
{
    return lhs.ToStdString() < rhs.ToStdString();
}

inline std::ostream& operator<<(std::ostream& os, const MovieName& movieName)
{
    os << movieName.ToStdString();
    return os;
}

inline std::istream& operator>>(std::istream& is, MovieName& movieName)
{
    auto s = std::string{};
    is >> s;
    movieName = MovieName{std::move(s)};
    return is;
}

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_MOVIENAME_INL_H
