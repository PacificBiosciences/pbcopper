// Author: Derek Barnett

#include <pbcopper/utility/CallbackTimer.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>

namespace PacBio {
namespace Utility {

// Implementation inspired from:
//    http://codereview.stackexchange.com/questions/40473/portable-periodic-one-shot-timer-implementation

using CallbackTimerClock = std::chrono::steady_clock;
using CallbackTimerTimePoint = std::chrono::time_point<CallbackTimerClock>;
using CallbackTimerDuration = std::chrono::milliseconds;
using CallbackTimerMutexLocker = std::unique_lock<std::mutex>;

// shared timer for single-requests
static std::unique_ptr<CallbackTimer> singleShotTimer = nullptr;

// mutex for protecting its initialization
static std::mutex singleShotSync;

class CallbackTimerJob
{
public:
    CallbackTimer::JobId id;
    CallbackTimerTimePoint next;
    CallbackTimerDuration period;
    CallbackTimer::HandlerFn handler;
    bool running;

    explicit CallbackTimerJob(CallbackTimer::JobId id_ = 0) : id(id_), running(false) {}

    template <typename CallbackType>
    CallbackTimerJob(CallbackTimer::JobId id_, CallbackTimerTimePoint next_,
                     CallbackTimerDuration period_, CallbackType&& handler_) noexcept
        : id(id_)
        , next(next_)
        , period(period_)
        , handler(std::forward<CallbackType>(handler_))
        , running(false)
    {
    }

    CallbackTimerJob(CallbackTimerJob&& r) noexcept
        : id(r.id)
        , next(r.next)
        , period(r.period)
        , handler(std::move(r.handler))
        , running(r.running)
    {
    }

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
    bool operator()(const CallbackTimerJob& a, const CallbackTimerJob& b) const
    {
        return a.next < b.next;
    }
};

class CallbackTimer::CallbackTimerPrivate
{
    using JobId = CallbackTimer::JobId;
    using HandlerFn = CallbackTimer::HandlerFn;
    using JobMap = std::unordered_map<JobId, CallbackTimerJob>;
    using QueueValue = std::reference_wrapper<CallbackTimerJob>;
    using JobQueue = std::multiset<QueueValue, NextActive>;

public:
    CallbackTimerPrivate(void)
        : nextId(1)
        , queue(NextActive())
        , worker(std::bind(&CallbackTimerPrivate::Run, this))
        , done(false)
    {
    }

    ~CallbackTimerPrivate(void)
    {
        CallbackTimerMutexLocker lock(sync);

        // mark ourselves as 'done' & wait for thread to wrap up
        done = true;
        wakeUp.notify_all();
        lock.unlock();
        worker.join();
    }

    JobId Schedule(const uint64_t msFromNow, const uint64_t msPeriod, const HandlerFn& handler)
    {
        return ScheduleImpl(
            CallbackTimerJob{0, CallbackTimerClock::now() + CallbackTimerDuration(msFromNow),
                             CallbackTimerDuration(msPeriod), handler});
    }

    JobId Schedule(const uint64_t msFromNow, const uint64_t msPeriod, HandlerFn&& handler)
    {
        return ScheduleImpl(
            CallbackTimerJob{0, CallbackTimerClock::now() + CallbackTimerDuration(msFromNow),
                             CallbackTimerDuration(msPeriod), std::move(handler)});
    }

    bool IsActive(const JobId id)
    {
        CallbackTimerMutexLocker lock(sync);
        return activeJobs.find(id) != activeJobs.end();
    }

    bool Cancel(const JobId id)
    {
        CallbackTimerMutexLocker lock(sync);
        auto i = activeJobs.find(id);
        if (i == activeJobs.end())
            return false;
        else if (i->second.running) {
            // A callback is in progress for this Instance,
            // so flag it for deletion in the worker
            i->second.running = false;
        } else {
            queue.erase(std::ref(i->second));
            activeJobs.erase(i);
        }

        wakeUp.notify_all();
        return true;
    }

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

    // this is our main thread worker, processing the job queue & calling the
    // callback functions
    //
    void Run(void)
    {
        CallbackTimerMutexLocker lock(sync);
        while (!done) {
            if (queue.empty()) {
                // Wait (forever) for work
                wakeUp.wait(lock);
            } else {
                auto firstJob = queue.begin();
                CallbackTimerJob& instance = *firstJob;
                auto now = CallbackTimerClock::now();
                if (now >= instance.next) {
                    queue.erase(firstJob);

                    // Mark it as running to handle racing destroy
                    instance.running = true;

                    // Call the handler
                    lock.unlock();
                    instance.handler();
                    lock.lock();

                    if (done) {
                        break;
                    } else if (!instance.running) {
                        // Running was set to false, destroy was called
                        // for this Instance while the callback was in progress
                        // (this thread was not holding the lock during the callback)
                        activeJobs.erase(instance.id);
                    } else {
                        instance.running = false;

                        // If it is periodic, schedule a new one
                        if (instance.period.count() > 0) {
                            instance.next = instance.next + instance.period;
                            queue.insert(instance);
                        } else {
                            activeJobs.erase(instance.id);
                        }
                    }
                } else {
                    // Wait until the timer is ready or a timer creation notifies
                    wakeUp.wait_until(lock, instance.next);
                }
            }
        }
    }

    JobId ScheduleImpl(CallbackTimerJob&& item)
    {
        CallbackTimerMutexLocker lock(sync);
        item.id = nextId++;
        auto iter = activeJobs.emplace(item.id, std::move(item));
        queue.insert(iter.first->second);
        wakeUp.notify_all();
        return item.id;
    }
};

CallbackTimer::CallbackTimer(void) : d_(new CallbackTimerPrivate) {}

CallbackTimer::~CallbackTimer(void) {}

bool CallbackTimer::Cancel(const JobId id) { return d_->Cancel(id); }

bool CallbackTimer::IsActive(const JobId id) { return d_->IsActive(id); }

CallbackTimer::JobId CallbackTimer::Schedule(const uint64_t msFromNow, const uint64_t msPeriod,
                                             const HandlerFn& handler)
{
    return d_->Schedule(msFromNow, msPeriod, handler);
}

CallbackTimer::JobId CallbackTimer::Schedule(const uint64_t msFromNow, const uint64_t msPeriod,
                                             HandlerFn&& handler)
{
    return d_->Schedule(msFromNow, msPeriod, std::move(handler));
}

void CallbackTimer::SingleShot(uint64_t when, const HandlerFn& handler)
{
    CallbackTimerMutexLocker lock(singleShotSync);
    if (!singleShotTimer) singleShotTimer.reset(new CallbackTimer);
    singleShotTimer->Schedule(when, 0, handler);
}

void CallbackTimer::SingleShot(uint64_t when, HandlerFn&& handler)
{
    CallbackTimerMutexLocker lock(singleShotSync);
    if (!singleShotTimer) singleShotTimer.reset(new CallbackTimer);
    singleShotTimer->Schedule(when, 0, std::move(handler));
}

}  // namespace Utility
}  // namespace PacBio
