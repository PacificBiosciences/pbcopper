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

#ifndef PBCOPPER_UTILITY_CALLBACKTIMER_H
#define PBCOPPER_UTILITY_CALLBACKTIMER_H

#include <cstdint>
#include <functional>
#include <memory>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

namespace internal {
class CallbackTimerPrivate;
}

/// \brief The CallbackTimer class provides repetitive and single-shot timers
///        for scheduling asynchronous tasks.
///
/// This class is used to schedule one or more callbacks to fire at some point
/// number of milliseconds in the future, and optionally fire again every so
/// many milliseconds.
///
/// As it stands, the callback function must be of the signature:
///
///     void foo(void);
///
/// whether a free function, member function, or lambda.
///
class CallbackTimer
{
public:
    typedef uint64_t JobId;
    typedef std::function<void()> HandlerFn;

public:
    static void SingleShot(uint64_t when, const HandlerFn& handler);
    static void SingleShot(uint64_t when, HandlerFn&& handler);

public:
    CallbackTimer(void);
    ~CallbackTimer(void);

public:
    JobId Schedule(const uint64_t when, const uint64_t period, const HandlerFn& handler);

    JobId Schedule(const uint64_t when, const uint64_t period, HandlerFn&& handler);

public:
    bool Cancel(const JobId id);
    bool IsActive(const JobId id);

private:
    std::unique_ptr<internal::CallbackTimerPrivate> d_;
};

}  // namespace Utility
}  // namespace PacBio

#include <pbcopper/utility/internal/CallbackTimer-inl.h>

#endif  // PBCOPPER_UTILITY_CALLBACKTIMER_H
