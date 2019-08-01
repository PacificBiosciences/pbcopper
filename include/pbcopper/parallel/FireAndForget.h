// Author: Lance Hepler & Armin Töpfer

#ifndef PBCOPPER_PARALLEL_FIREANDFORGET_H
#define PBCOPPER_PARALLEL_FIREANDFORGET_H

#include <pbcopper/PbcopperConfig.h>

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <future>
#include <mutex>
#include <queue>

#include <boost/optional.hpp>

namespace PacBio {
namespace Parallel {

class FireAndForget
{
private:
    typedef boost::optional<std::packaged_task<void()>> TTask;

public:
    FireAndForget(const size_t size, const size_t mul = 2)
        : exc{nullptr}, sz{size * mul}, abort{false}
    {
        for (size_t i = 0; i < size; ++i) {
            threads.emplace_back(std::thread([this]() {
                // Get the first task per thread
                TTask task = PopTask();
                do {
                    try {
                        // Check if queue should be aborted and
                        // if there is a task
                        if (!abort && task) {
                            // Execute task
                            (*task)();
                            // Check for the return value / exception
                            task->get_future().get();
                        };
                    } catch (...) {
                        std::lock_guard<std::mutex> g(m);
                        // If there is an exception, signal to abort queue
                        abort = true;
                        // And store exception
                        exc = std::current_exception();
                        popped.notify_one();
                    }
                    task = PopTask();
                } while (!abort && task);  // Stop if there are no tasks or abort has been signaled
            }));
        }
    }

    template <typename F, typename... Args>
    void ProduceWith(F&& f, Args&&... args)
    {
        std::packaged_task<void()> task{std::bind(std::forward<F>(f), std::forward<Args>(args)...)};

        {
            std::unique_lock<std::mutex> lk(m);
            popped.wait(lk, [&task, this]() {
                if (exc) std::rethrow_exception(exc);

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
        {
            std::lock_guard<std::mutex> g(m);
            // Push boost::none to signal that there are no further tasks
            head.emplace(boost::none);
        }
        // Let all workers know that they should look that there is no further work
        pushed.notify_all();

        // Wait for all threads to join and do not continue before all tasks
        // have been finished.
        for (auto& thread : threads)
            thread.join();

        // Is there a final exception, throw if so..
        if (exc) std::rethrow_exception(exc);
    }

private:
    TTask PopTask()
    {
        TTask task(boost::none);

        {
            std::unique_lock<std::mutex> lk(m);
            pushed.wait(lk, [&task, this]() {
                if (head.empty()) return false;

                if ((task = std::move(head.front()))) {
                    head.pop();
                }

                return true;
            });
        }
        popped.notify_one();

        return task;
    }

    std::vector<std::thread> threads;
    std::queue<TTask> head;
    std::condition_variable popped;
    std::condition_variable pushed;
    std::exception_ptr exc;
    std::mutex m;
    size_t sz;
    std::atomic_bool abort;
};

}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_FIREANDFORGET_H
