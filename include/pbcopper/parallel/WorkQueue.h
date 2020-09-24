// Author: Lance Hepler

#ifndef PBCOPPER_PARALLEL_WORKQUEUE_H
#define PBCOPPER_PARALLEL_WORKQUEUE_H

#include <pbcopper/PbcopperConfig.h>

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <exception>
#include <future>
#include <mutex>
#include <vector>

#include <boost/optional.hpp>

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
    using TTask = boost::optional<std::packaged_task<T()>>;
    using TFuture = boost::optional<std::future<T>>;

public:
    WorkQueue(const size_t size, const size_t mul = 2)
        : exc{nullptr}, sz{size * mul}, abort{false}, thrown{false}, workersFinalized{false}
    {
        for (size_t i = 0; i < size; ++i) {
            threads.emplace_back(std::thread([this]() {
                try {
                    if (abort) return;
                    while (auto task = PopTask()) {
                        if (abort) return;
                        (*task)();
                    }
                } catch (...) {
                    {
                        std::lock_guard<std::mutex> g(m);
                        exc = std::current_exception();
                    }
                    popped.notify_all();
                }
            }));
        }
    }

    ~WorkQueue() noexcept(false)
    {
        if (exc && !thrown) std::rethrow_exception(exc);
    }

    template <typename F, typename... Args>
    void ProduceWith(F&& f, Args&&... args)
    {
        std::packaged_task<T()> task{std::bind(std::forward<F>(f), std::forward<Args>(args)...)};

        {
            std::unique_lock<std::mutex> lk(m);
            popped.wait(lk, [&task, this]() {
                if (exc) {
                    thrown = true;
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
                if (tail.empty()) return false;
                tail.swap(futs);
                return !futs.empty();
            });
        }
        pushed.notify_one();

        try {
            for (auto& fut : futs) {
                if (!fut) return false;
                auto result = fut->get();
                cont(std::forward<Args>(args)..., std::move(result));
            }
            return true;
        } catch (...) {
            abort = true;
            {
                std::lock_guard<std::mutex> g(m);
                exc = std::current_exception();
            }
            popped.notify_all();
        }
        return false;
    }

    void FinalizeWorkers()
    {
        if (!workersFinalized) {
            {
                std::lock_guard<std::mutex> g(m);
                // Push boost::none to signal that there are no further tasks
                head.emplace_back(boost::none);
            }
            // Let all workers know that they should look that there is no further work
            pushed.notify_all();
            workersFinalized = true;
        }
    }

    void Finalize()
    {
        FinalizeWorkers();
        // One last notify in case a consumer exception occured and threads
        // are still waiting in PopTask wait.
        pushed.notify_all();
        // Wait for all threads to join and do not continue before all tasks
        // have been finished.
        for (auto& thread : threads)
            thread.join();

        // Is there a final exception, throw if so..
        if (exc) {
            thrown = true;
            std::rethrow_exception(exc);
        }
    }

private:
    TTask PopTask()
    {
        TTask task(boost::none);

        {
            std::unique_lock<std::mutex> lk(m);
            pushed.wait(lk, [&task, this]() {
                // If a consumer task threw an exception, this is the shortcut
                // to escape
                if (abort) return true;

                if (head.empty() || tail.size() >= sz) return false;

                if ((task = std::move(head.front()))) {
                    head.pop_front();
                    tail.emplace_back(std::move(task->get_future()));
                } else
                    tail.emplace_back(boost::none);

                return true;
            });
        }
        popped.notify_one();  // ProduceWith/ConsumeWith may be waiting
        pushed.notify_one();  // other PopTask's may also be waiting

        return task;
    }

    std::vector<std::thread> threads;
    std::deque<TTask> head;
    std::deque<TFuture> tail;
    std::condition_variable popped;
    std::condition_variable pushed;
    std::exception_ptr exc;
    std::mutex m;
    size_t sz;
    std::atomic_bool abort;
    std::atomic_bool thrown;
    std::atomic_bool workersFinalized;
};
}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_WORKQUEUE_H
