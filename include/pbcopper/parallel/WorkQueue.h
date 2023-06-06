#ifndef PBCOPPER_PARALLEL_WORKQUEUE_H
#define PBCOPPER_PARALLEL_WORKQUEUE_H

#include <pbcopper/PbcopperConfig.h>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <vector>

#include <cstddef>

namespace PacBio {
namespace Parallel {

/// To properly shut down the workqueue, call methods in this order:
///    workQueue.FinalizeWorkers();
///    workerThread.wait(); // Shut down the consuming worker thread!
///    workQueue.Finalize();
template <typename T>
class WorkQueue
{
private:
    using TTask = std::optional<std::packaged_task<T()>>;
    using TFuture = std::optional<std::future<T>>;

public:
    WorkQueue(const std::size_t size, const std::size_t mul = 2)
        : exc{nullptr}
        , sz{size * mul}
        , abort{false}
        , thrown{false}
        , workersFinalized{false}
        , acceptingJobs{true}
    {
        for (std::size_t i = 0; i < size; ++i) {
            threads.emplace_back(std::thread([this]() {
                try {
                    if (abort) {
                        return;
                    }
                    while (auto task = PopTask()) {
                        if (abort) {
                            return;
                        }
                        (*task)();
                    }
                } catch (...) {
                    SetFirstException();
                    popped.notify_all();
                }
            }));
        }
    }

    ~WorkQueue() noexcept(false) { Finalize(); }

    template <typename F, typename... Args>
    void ProduceWith(F&& f, Args&&... args)
    {
        if (!acceptingJobs) {
            throw std::runtime_error(
                "WorkQueue error: Cannot dispatch jobs to finalized work queue!");
        }

        std::packaged_task<T()> task{std::bind(std::forward<F>(f), std::forward<Args>(args)...)};

        // Throw exception every time if abort has been signaled
        if (abort) {
            std::rethrow_exception(exc);
        }

        {
            std::unique_lock<std::mutex> lk(m);
            popped.wait(lk, [&task, this]() {
                if (abort) {
                    std::rethrow_exception(exc);
                }

                if (head.size() < sz) {
                    head.emplace_back(std::move(task));
                    return true;
                }

                return false;
            });
        }
        pushed.notify_one();
    }

    template <typename F, typename... Args>
    bool ConsumeWith(F&& cont, Args&&... args)
    {
        std::deque<TFuture> futs;

        {
            std::unique_lock<std::mutex> lk(m);
            popped.wait(lk, [&futs, this]() {
                if (tail.empty()) {
                    return false;
                }
                tail.swap(futs);
                return !futs.empty();
            });
        }
        pushed.notify_one();

        try {
            for (auto& fut : futs) {
                if (!fut) {
                    return false;
                }
                auto result = fut->get();
                cont(std::forward<Args>(args)..., std::move(result));
            }
            return true;
        } catch (...) {
            SetFirstException();
            popped.notify_all();
        }
        return false;
    }

    void FinalizeWorkers()
    {
        // Only finalize workers once
        std::call_once(finalizeWorkersOnceFlag, [&]() {
            acceptingJobs = false;
            {
                std::lock_guard<std::mutex> g(m);
                // Push sentinel to signal that there are no further tasks
                head.emplace_back();
            }
            // Let all workers know that they should look that there is no further work
            pushed.notify_all();
        });
    }

    void Finalize()
    {
        // Only finalize once
        std::call_once(finalizeOnceFlag, [&]() {
            FinalizeWorkers();
            // One last notify in case a consumer exception occured and threads
            // are still waiting in PopTask wait.
            pushed.notify_all();
            // Wait for all threads to join and do not continue before all tasks
            // have been finished.
            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        });

        // Is there a final exception, throw once. This avoids throwing in the
        // destructor if Finalize() has been called before.
        if (abort && !thrown) {
            thrown = true;
            std::rethrow_exception(exc);
        }
    }

private:
    TTask PopTask()
    {
        TTask task;

        {
            std::unique_lock<std::mutex> lk(m);
            pushed.wait(lk, [&task, this]() {
                // If a consumer task threw an exception, this is the shortcut
                // to escape
                if (abort) {
                    return true;
                }

                if (head.empty() || tail.size() >= sz) {
                    return false;
                }

                if ((task = std::move(head.front()))) {
                    head.pop_front();
                    tail.emplace_back(std::move(task->get_future()));
                } else {
                    tail.emplace_back();
                }

                return true;
            });
        }
        popped.notify_one();  // ProduceWith/ConsumeWith may be waiting
        pushed.notify_one();  // other PopTask's may also be waiting

        return task;
    }

    void SetFirstException()
    {
        // Only store the first exception
        std::call_once(exceptionOnceFlag, [&]() {
            std::unique_lock<std::mutex> lk(m);
            // Store exception to be rethrown later
            exc = std::current_exception();
            // Signal to abort queue
            abort = true;
        });
    }

    std::vector<std::thread> threads;
    std::deque<TTask> head;
    std::deque<TFuture> tail;
    std::condition_variable popped;
    std::condition_variable pushed;
    std::exception_ptr exc;
    std::once_flag exceptionOnceFlag;
    std::once_flag finalizeOnceFlag;
    std::once_flag finalizeWorkersOnceFlag;
    std::mutex m;
    std::size_t sz;
    std::atomic_bool abort;
    std::atomic_bool thrown;
    std::atomic_bool workersFinalized;
    std::atomic_bool acceptingJobs;
};
}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_WORKQUEUE_H
