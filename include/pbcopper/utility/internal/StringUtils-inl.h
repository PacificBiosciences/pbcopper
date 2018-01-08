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

#ifndef PBCOPPER_UTILITY_STRINGUTILS_INL_H
#define PBCOPPER_UTILITY_STRINGUTILS_INL_H

#include <cstddef>
#include <sstream>

#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace Utility {

inline std::string Join(const std::vector<std::string>& input,
                        const std::string& delim)
{
    // determine total joined length
    size_t totalLen = 0;
    for (const auto& s : input)
        totalLen += s.size();
    if (!input.empty())
        totalLen += delim.size() * (input.size()-1); // no delim after last string

    // join input strings
    std::string result;
    result.reserve(totalLen);
    for (size_t i = 0; i < input.size(); ++i) {
        if (i != 0)
            result += delim;
        result += input.at(i);
    }
    return result;
}

inline std::string Join(const std::vector<std::string>& input,
                        const char delim)
{
    return Join(input, std::string(1, delim));
}

inline std::vector<std::string> Split(const std::string& line,
                                      const char delim)
{
    std::vector<std::string> tokens;
    std::stringstream lineStream{ line };
    std::string token;
    while (std::getline(lineStream, token, delim))
        tokens.push_back(token);
    return tokens;
}

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_STRINGUTILS_INL_H
