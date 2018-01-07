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

#include <pbcopper/data/MovieName.h>

#include <cstddef>

namespace PacBio {
namespace Data {

MovieName::MovieName(const std::string& instrumentName,
                     const std::string& runStartTime)
    : partsCache_(nullptr)
{
    // construct name from parts
    auto result = std::string{ };
    result.reserve(128);
    result += "m";
    result += instrumentName;
    result += "_";
    result += runStartTime;
    movieName_ = result;
    // don't update cache until actually requested
}

void MovieName::UpdatePartsCache(void) const
{
    // sanity checks
    assert(partsCache_ == nullptr);
    if (movieName_.empty())
        return;

    // calculate name parts
    const char underscore = '_';
    const size_t firstUnderscore  = movieName_.find(underscore);

    const char* movieCStr  = movieName_.c_str();
    const char* nameStart = movieCStr + 1;
    const char* rstStart  = movieCStr + firstUnderscore + 1; // skip '_'
    const size_t nameSize = (firstUnderscore - 1);
    const size_t rstSize  = (movieName_.size() - firstUnderscore) - 1; // skip '\0 '

    // cache name parts
    partsCache_.reset(new PartsCache
    {
        boost::string_ref{ nameStart, nameSize }, // instrumentName
        boost::string_ref{ rstStart, rstSize }    // runStartTime
    });

    // checks - here? or elsewhere?
    if (partsCache_->instrumentName_.empty())
        throw std::runtime_error("MovieName: instrument name must not be empty");
    if (partsCache_->runStartTime_.empty())
        throw std::runtime_error("MovieName: run start time must not be empty");
}

} // namespace Data
} // namespace PacBio
