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
