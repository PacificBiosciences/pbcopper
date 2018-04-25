// Author: Derek Barnett

#include <pbcopper/utility/CallbackTimer.h>

namespace PacBio {
namespace Utility {

// Implementation inspired from:
//    http://codereview.stackexchange.com/questions/40473/portable-periodic-one-shot-timer-implementation

namespace internal {

typedef std::chrono::steady_clock CallbackTimerClock;
typedef std::chrono::time_point<CallbackTimerClock> CallbackTimerTimePoint;
typedef std::chrono::milliseconds CallbackTimerDuration;
typedef std::unique_lock<std::mutex> CallbackTimerMutexLocker;

CallbackTimer::JobId CallbackTimerPrivate::ScheduleImpl(CallbackTimerJob&& item)
{
    CallbackTimerMutexLocker lock(sync);
    item.id = nextId++;
    auto iter = activeJobs.emplace(item.id, std::move(item));
    queue.insert(iter.first->second);
    wakeUp.notify_all();
    return item.id;
}

bool CallbackTimerPrivate::Cancel(const CallbackTimer::JobId id)
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

// this is our main thread worker, processing the job queue & calling the
// callback functions
//
void CallbackTimerPrivate::Run(void)
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

// shared timer for single-requests
static std::unique_ptr<CallbackTimer> singleShotTimer = nullptr;

// mutex for protecting its initialization
static std::mutex singleShotSync;

}  // namespace internal

void CallbackTimer::SingleShot(uint64_t when, const HandlerFn& handler)
{
    internal::CallbackTimerMutexLocker lock(internal::singleShotSync);
    if (!internal::singleShotTimer) internal::singleShotTimer.reset(new CallbackTimer);
    internal::singleShotTimer->Schedule(when, 0, handler);
}

void CallbackTimer::SingleShot(uint64_t when, HandlerFn&& handler)
{
    internal::CallbackTimerMutexLocker lock(internal::singleShotSync);
    if (!internal::singleShotTimer) internal::singleShotTimer.reset(new CallbackTimer);
    internal::singleShotTimer->Schedule(when, 0, std::move(handler));
}

}  // namespace Utility
}  // namespace PacBio
