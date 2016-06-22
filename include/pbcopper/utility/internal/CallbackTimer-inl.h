#ifndef PBCOPPER_UTILITY_CALLBACKTIMER_INL_H
#define PBCOPPER_UTILITY_CALLBACKTIMER_INL_H

#include "pbcopper/utility/CallbackTimer.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>

namespace PacBio {
namespace Utility {
namespace internal {

typedef std::chrono::steady_clock                    CallbackTimerClock;
typedef std::chrono::time_point<CallbackTimerClock>  CallbackTimerTimePoint;
typedef std::chrono::milliseconds                    CallbackTimerDuration;
typedef std::unique_lock<std::mutex>                 CallbackTimerMutexLocker;

struct CallbackTimerJob
{
public:
    CallbackTimer::JobId     id;
    CallbackTimerTimePoint   next;
    CallbackTimerDuration    period;
    CallbackTimer::HandlerFn handler;
    bool running;

public:
    CallbackTimerJob(CallbackTimer::JobId id = 0)
        : id(id)
        , running(false)
    { }

    template<typename CallbackType>
    CallbackTimerJob(CallbackTimer::JobId id,
                     CallbackTimerTimePoint next,
                     CallbackTimerDuration period,
                     CallbackType&& handler) noexcept
        : id(id)
        , next(next)
        , period(period)
        , handler(std::forward<CallbackType>(handler))
        , running(false)
    { }

    CallbackTimerJob(CallbackTimerJob&& r) noexcept
        : id(r.id)
        , next(r.next)
        , period(r.period)
        , handler(std::move(r.handler))
        , running(r.running)
    { }

    CallbackTimerJob& operator=(CallbackTimerJob&& r)
    {
        if (this != &r) {
            id = r.id;
            next = r.next;
            period = r.period;
            handler = std::move(r.handler);
            running = r.running;
        }
        return *this;
    }

    CallbackTimerJob(CallbackTimerJob const& r) = delete;
    CallbackTimerJob& operator=(CallbackTimerJob const& r) = delete;
};

struct NextActive
{
    bool operator()(const CallbackTimerJob& a,
                    const CallbackTimerJob& b) const
    { return a.next < b.next; }
};

class CallbackTimerPrivate
{
    typedef CallbackTimer::JobId                         JobId;
    typedef CallbackTimer::HandlerFn                     HandlerFn;
    typedef std::unordered_map<JobId, CallbackTimerJob>  JobMap;
    typedef std::reference_wrapper<CallbackTimerJob>     QueueValue;
    typedef std::multiset<QueueValue, NextActive>        JobQueue;

public:
    CallbackTimerPrivate(void)
        : nextId(1)
        , queue(NextActive())
        , worker(std::bind(&CallbackTimerPrivate::Run, this))
        , done(false)
    { }

    ~CallbackTimerPrivate(void)
    {
        CallbackTimerMutexLocker lock(sync);

        // mark ourselves as 'done' & wait for thread to wrap up
        done = true;
        wakeUp.notify_all();
        lock.unlock();
        worker.join();
    }

public:
    JobId Schedule(const uint64_t msFromNow,
                   const uint64_t msPeriod,
                   const HandlerFn& handler)
    {
        return ScheduleImpl(
            CallbackTimerJob{
                0,
                CallbackTimerClock::now() + CallbackTimerDuration(msFromNow),
                CallbackTimerDuration(msPeriod),
                handler
            }
        );
    }

    JobId Schedule(const uint64_t msFromNow,
                   const uint64_t msPeriod,
                   HandlerFn&& handler)
    {
        return ScheduleImpl(
            CallbackTimerJob{
                0,
                CallbackTimerClock::now() + CallbackTimerDuration(msFromNow),
                CallbackTimerDuration(msPeriod),
                std::move(handler)
            }
        );
    }


    bool IsActive(const JobId id)
    {
        CallbackTimerMutexLocker lock(sync);
        return activeJobs.find(id) != activeJobs.end();
    }

    bool Cancel(const JobId id);

private:
    // job storage
    JobId nextId;
    JobMap activeJobs;
    JobQueue queue;

    // job scheduling
    std::mutex sync;
    std::condition_variable wakeUp;
    std::thread worker;
    bool done;

private:
    void Run(void);
    JobId ScheduleImpl(CallbackTimerJob&& item);
};

} // namespace internal

inline CallbackTimer::CallbackTimer(void)
    : d_(new internal::CallbackTimerPrivate)
{ }

inline CallbackTimer::~CallbackTimer(void) { }

inline bool CallbackTimer::Cancel(const JobId id)
{
    return d_->Cancel(id);
}

inline bool CallbackTimer::IsActive(const JobId id)
{
    return d_->IsActive(id);
}

inline CallbackTimer::JobId CallbackTimer::Schedule(const uint64_t msFromNow,
                                                    const uint64_t msPeriod,
                                                    const HandlerFn &handler)
{
    return d_->Schedule(msFromNow, msPeriod, handler);
}

inline CallbackTimer::JobId CallbackTimer::Schedule(const uint64_t msFromNow,
                                                    const uint64_t msPeriod,
                                                    HandlerFn &&handler)
{
    return d_->Schedule(msFromNow, msPeriod, std::move(handler));
}

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_CALLBACKTIMER_INL_H
