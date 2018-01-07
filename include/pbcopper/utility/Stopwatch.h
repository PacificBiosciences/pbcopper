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

#ifndef PBCOPPER_UTILITY_STOPWATCH_H
#define PBCOPPER_UTILITY_STOPWATCH_H

#include <chrono>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

class Stopwatch
{
public:
    /// \name Constructors & Related Methods
    /// \{

    /// Creates a stopwatch and begins timing.
    Stopwatch(void);

    Stopwatch(const Stopwatch& other) = default;
    Stopwatch(Stopwatch&& other) = default;
    Stopwatch& operator=(const Stopwatch& other) = default;
    Stopwatch& operator=(Stopwatch& other) = default;
    ~Stopwatch(void) = default;

    /// \}

public:

    /// \returns the elapsed time (in milliseconds) since timing began.
    float ElapsedMilliseconds(void) const;

    /// \returns the elapsed time (in seconds) since timing began.
    float ElapsedSeconds(void) const;

    /// \returns the elapsed time (in user-provided units) since timing began.
    template<typename TimeUnit>
    float Elapsed(void) const;

    // resets internal values
    void Reset(void);

private:
    std::chrono::steady_clock::time_point start_;
};

} // namespace Utility
} // namespace PacBio

#include <pbcopper/utility/internal/Stopwatch-inl.h>

#endif // PBCOPPER_UTILITY_STOPWATCH_H
