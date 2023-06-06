#ifndef PBCOPPER_PARALLEL_FIREANDFORGET_H
#define PBCOPPER_PARALLEL_FIREANDFORGET_H

#include <pbcopper/PbcopperConfig.h>

#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>

#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace Parallel {

class FireAndForget
{
private:
    typedef std::optional<std::packaged_task<void()>> TTask;

public:
    FireAndForget(const std::size_t size, const std::size_t mul = 2)
        : exc{nullptr}
        , numThreads{size}
        , sz{size * mul}
        , abort{false}
        , thrown{false}
        , acceptingJobs{true}
    {
        for (std::size_t i = 0; i < size; ++i) {
            threads.emplace_back(std::thread([this]() {
                TTask task;
                do {
                    try {
                        task = PopTask();
                        // Check if queue should be aborted and
                        // if there is a task
                        if (!abort && task) {
                            // Execute task
                            (*task)();
                            // Check for the return value / exception
                            task->get_future().get();
                        };
                    } catch (...) {
                        SetFirstException();
                        popped.notify_one();
                    }
                } while (!abort && task);  // Stop if there are no tasks or abort has been signaled
            }));
        }
    }

    ~FireAndForget() noexcept(false)
    {
        if (acceptingJobs) {
            Finalize();
        }
        if (exc && !thrown) {
            std::rethrow_exception(exc);
        }
    }

    template <typename F, typename... Args>
    void ProduceWith(F&& f, Args&&... args)
    {
        if (!acceptingJobs) {
            throw std::runtime_error(
                "FireAndForget error: Cannot dispatch jobs to finalized thread pool!");
        }

        std::packaged_task<void()> task{std::bind(std::forward<F>(f), std::forward<Args>(args)...)};

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
        acceptingJobs = false;
        {
            std::lock_guard<std::mutex> g(m);
            // Push sentinel to signal that there are no further tasks
            head.emplace();
        }
        // Let all workers know that they should look that there is no further work
        pushed.notify_all();

        // Wait for all threads to join and do not continue before all tasks
        // have been finished.
        for (auto& thread : threads) {
            thread.join();
        }

        // Is there a final exception, throw if so..
        if (exc) {
            thrown = true;
            std::rethrow_exception(exc);
        }
    }

    std::size_t NumThreads() const { return numThreads; }

private:
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
    std::mutex m;
    std::size_t numThreads;
    std::size_t sz;
    std::atomic_bool abort;
    std::atomic_bool thrown;
    std::atomic_bool acceptingJobs;
};

///
/// \brief Use an existing FireAndForget to dispatch [0, numEntries) callbacks.
///        Returns after all dispatched jobs finished.
///
/// \param faf         reference to FaF, nullptr allowed
/// \param numEntries  number of submissions
/// \param callback    function to be dispatched to FaF
///
void Dispatch(Parallel::FireAndForget* faf, std::int32_t numEntries,
              const std::function<void(std::int32_t)>& callback);

}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_FIREANDFORGET_H
