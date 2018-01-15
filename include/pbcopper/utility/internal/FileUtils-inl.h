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

#ifndef PBCOPPER_UTILITY_FILEUTILS_INL_H
#define PBCOPPER_UTILITY_FILEUTILS_INL_H

#include <sys/stat.h>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <fstream>

#include <boost/algorithm/string.hpp>

#include <pbcopper/utility/FileUtils.h>

namespace PacBio {
namespace Utility {

inline bool FileExists(const std::string& path)
{
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
}

inline std::string FileExtension(const std::string& path)
{
    size_t fileStart = path.find_last_of("/");

    if (fileStart == std::string::npos) fileStart = 0;

    // increment beyond the '/'
    ++fileStart;

    size_t extStart = path.substr(fileStart, path.length() - fileStart).find_last_of(".");

    if (extStart == std::string::npos) return "";

    // increment beyond '.'
    ++extStart;

    auto suffix = path.substr(fileStart + extStart, path.length() - fileStart - extStart);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
    return suffix;
}

inline std::string FilePrefix(const std::string& path)
{
    size_t fileStart = path.find_last_of("/");

    if (fileStart == std::string::npos) fileStart = -1;

    // increment beyond the '/'
    ++fileStart;

    size_t extStart = path.substr(fileStart, path.length() - fileStart).find_first_of(".");

    if (extStart == std::string::npos) return "";

    auto suffix = path.substr(fileStart, extStart);
    return suffix;
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_FILEUTILS_INL_H
