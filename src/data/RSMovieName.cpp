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

#include <pbcopper/data/RSMovieName.h>

#include <cstddef>

namespace PacBio {
namespace Data {

RSMovieName::RSMovieName(const std::string& runStartTime,
                         const std::string& instrumentSerialNumber,
                         const std::string& smrtCellBarcode,
                         const std::string& setNumber,
                         const std::string& partNumber)
    : partsCache_(nullptr)
{
    // construct name from parts
    auto result = std::string{ };
    result.reserve(128);
    result += "m";
    result += runStartTime;
    result += "_";
    result += instrumentSerialNumber;
    result += "_";
    result += smrtCellBarcode;
    result += "_";
    result += setNumber;
    result += "_";
    result += partNumber;
    movieName_ = result;

    // don't update cache until actually requested
}

void RSMovieName::UpdatePartsCache(void) const
{
    // sanity checks
    assert(partsCache_ == nullptr);
    if (movieName_.empty())
        return;

    // calculate name parts
    const char underscore = '_';
    const size_t firstUnderscore  = movieName_.find(underscore);
    const size_t secondUnderscore = movieName_.find(underscore, firstUnderscore+1);
    const size_t thirdUnderscore  = movieName_.find(underscore, secondUnderscore+1);
    const size_t fourthUnderscore = movieName_.find(underscore, thirdUnderscore+1);
    const size_t fifthUnderscore  = movieName_.find(underscore, fourthUnderscore+1);

    const char* movieCStr      = movieName_.c_str();
    const char* rstStart       = movieCStr + 1; // skip 'm' and include first '_'
    const char* serialNumStart = movieCStr + secondUnderscore + 1; // skip '_'
    const char* scbStart       = movieCStr + thirdUnderscore  + 1; // skip '_'
    const char* setNumStart    = movieCStr + fourthUnderscore + 1; // skip '_'
    const char* partNumStart   = movieCStr + fifthUnderscore  + 1; // skip '_'
    const size_t rstSize       = (secondUnderscore - 1);
    const size_t serialNumSize = (thirdUnderscore - secondUnderscore) - 1;  // skip '_'
    const size_t scbSize       = (fourthUnderscore - thirdUnderscore) - 1;  // skip '_'
    const size_t setNumSize    = (fifthUnderscore - fourthUnderscore) - 1;  // skip '_'
    const size_t partNumSize   = (movieName_.size() - fifthUnderscore) - 1; // skip '\0 '

    // cache name parts
    partsCache_.reset(new RSMovieName::PartsCache
    {
        boost::string_ref{ rstStart,       rstSize },       // runStartTime
        boost::string_ref{ serialNumStart, serialNumSize }, // serialNumber
        boost::string_ref{ scbStart,       scbSize },       // smrtCellBarcode
        boost::string_ref{ setNumStart,    setNumSize },    // setNumber
        boost::string_ref{ partNumStart,   partNumSize },   // partNumber
    });

    // checks - here? or elsewhere?
    if (partsCache_->runStartTime_.empty())
        throw std::runtime_error("RSMovieName: run start time must not be empty");
    if (partsCache_->serialNumber_.empty())
        throw std::runtime_error("RSMovieName: instrument serial number must not be empty");
    if (partsCache_->smrtCellBarcode_.empty())
        throw std::runtime_error("RSMovieName: SMRT cell barcode must not be empty");
    if (partsCache_->setNumber_.empty())
        throw std::runtime_error("RSMovieName: set number must not be empty");
    if (partsCache_->partNumber_.empty())
        throw std::runtime_error("RSMovieName: part number must not be empty");
}

} // namespace Data
} // namespace PacBio
