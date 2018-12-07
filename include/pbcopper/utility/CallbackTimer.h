// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_CALLBACKTIMER_H
#define PBCOPPER_UTILITY_CALLBACKTIMER_H

#include <cstdint>
#include <functional>
#include <memory>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

/// \brief The CallbackTimer class provides repetitive and single-shot timers
///        for scheduling asynchronous tasks.
///
/// This class is used to schedule one or more callbacks to fire at some point
/// number of milliseconds in the future, and optionally fire again every so
/// many milliseconds.
///
/// As it stands, the callback function must be of the signature:
///
///     void foo();
///
/// whether a free function, member function, or lambda.
///
class CallbackTimer
{
public:
    using JobId = uint64_t;
    using HandlerFn = std::function<void()>;

    static void SingleShot(uint64_t when, const HandlerFn& handler);
    static void SingleShot(uint64_t when, HandlerFn&& handler);

    CallbackTimer();
    ~CallbackTimer();

    JobId Schedule(const uint64_t when, const uint64_t period, const HandlerFn& handler);
    JobId Schedule(const uint64_t when, const uint64_t period, HandlerFn&& handler);

    bool Cancel(const JobId id);
    bool IsActive(const JobId id);

private:
    class CallbackTimerPrivate;
    std::unique_ptr<CallbackTimerPrivate> d_;
};

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_CALLBACKTIMER_H
