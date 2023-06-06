#ifndef PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H
#define PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H

#include <pbcopper/PbcopperConfig.h>

#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <queue>

#include <cstddef>

namespace PacBio {
namespace Parallel {

/**
 Differences from normal FireAndForget:

 * ProduceWith expects a function taking Index followed by user-defined Args.
 * The Index is from 0 to size-1.
 * Each thread knows its own Index.
 * As a result, each task can use its Index to look into a predefined vector (user-controlled).
 * A 'finish' function is called for each thread after Finalize() pushes the sentinel onto the queue.
 The 'finish' function is not really needed, but it's helpful for debugging. Or
 to summarize results / close output streams.

 Caution: If an exception is thrown by your task, the 'finish' function is not called!
*/
class FireAndForgetIndexed
{
public:
    using Index = std::size_t;
    using TFunc = std::function<void(Index)>;
    using TPTask = std::packaged_task<void(Index)>;

public:
    FireAndForgetIndexed(const std::size_t size, const std::size_t mul = 2,
                         TFunc finish = TFunc{[](Index) {}})
        : exc{nullptr}, sz{size * mul}, abort{false}, thrown{false}, acceptingJobs{true}
    {
        for (Index index = 0; index < size; ++index) {
            threads.emplace_back(std::thread([this, finish, index]() {
                // Get the first task per thread
                TTask task;
                do {
                    try {
                        task = PopTask();
                        // Check if queue should be aborted and
                        // if there is a task
                        if (!abort && task) {
                            // Execute task
                            (*task)(index);
                            // Check for the return value / exception
                            task->get_future().get();
                        };
                    } catch (...) {
                        SetFirstException();
                        popped.notify_one();
                    }
                } while (!abort && task);  // Stop if there are no tasks or abort has been signaled
                try {
                    if (!abort) {
                        finish(index);
                    }
                } catch (...) {
                    SetFirstException();
                }
            }));
        }
    }

    ~FireAndForgetIndexed() noexcept(false) { Finalize(); }

    template <typename F, typename... Args>
    void ProduceWith(F&& f, Args&&... args)
    {
        if (!acceptingJobs) {
            throw std::runtime_error(
                "FireAndForgetIndexed error: Cannot dispatch jobs to finalized thread pool!");
        }

        // Create a function taking Index, which delegates to
        // a function taking Index followed by args.
        TPTask task{
            std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Args>(args)...)};

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
                    head.emplace(std::move(task));
                    return true;
                }

                return false;
            });
        }
        pushed.notify_one();
    }

    void Finalize()
    {
        // Only finalize once
        std::call_once(finalizeOnceFlag, [&]() {
            acceptingJobs = false;
            {
                std::lock_guard<std::mutex> g(m);
                // Push sentinel to signal that there are no further tasks
                head.emplace();
            }
            // Let all workers know that they should look that there is no further work
            pushed.notify_all();

            // Wait for all threads to join and do not continue before all tasks
            // have been finishshed.
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
    using TTask = std::optional<TPTask>;

    TTask PopTask()
    {
        TTask task;

        {
            std::unique_lock<std::mutex> lk(m);
            pushed.wait(lk, [&task, this]() {
                if (head.empty()) {
                    return false;
                }

                if ((task = std::move(head.front()))) {
                    head.pop();
                }

                return true;
            });
        }
        popped.notify_one();

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
    std::queue<TTask> head;
    std::condition_variable popped;
    std::condition_variable pushed;
    std::exception_ptr exc;
    std::once_flag exceptionOnceFlag;
    std::once_flag finalizeOnceFlag;
    std::mutex m;
    std::size_t sz;
    std::atomic_bool abort;
    std::atomic_bool thrown;
    std::atomic_bool acceptingJobs;
};

}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H
