// Author: Lance Hepler & Armin Töpfer

#ifndef PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H
#define PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H

#include <pbcopper/PbcopperConfig.h>

#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <queue>

#include <boost/optional.hpp>

namespace PacBio {
namespace Parallel {

/**
 Differences from normal FireAndForget:

 * ProduceWith expects a function taking Index followed by user-defined Args.
 * The Index is from 0 to size-1.
 * Each thread knows its own Index.
 * As a result, each task can use its Index to look into a predefined vector (user-controlled).
 * A 'fini' function is called for each thread after Finalize() pushes the sentinel onto the queue.
 
 The 'fini' function is not really needed, but it's helpful for debugging.
*/
class FireAndForgetIndexed
{
public:
    using Index = size_t;
    using TFunc = std::function<void(Index)>;
    using TPTask = std::packaged_task<void(Index)>;

public:
    FireAndForgetIndexed(const size_t size, const size_t mul = 2, TFunc fini = TFunc{[](Index) {}})
        : exc{nullptr}, sz{size * mul}
    {
        for (Index index = 0; index < size; ++index) {
            threads.emplace_back(std::thread([this, fini, index]() {
                try {
                    while (auto task = PopTask()) {
                        (*task)(index);
                    }
                    fini(index);
                } catch (...) {
                    {
                        std::lock_guard<std::mutex> g(m);
                        exc = std::current_exception();
                    }
                    popped.notify_one();
                }
            }));
        }
    }

    ~FireAndForgetIndexed()
    {
        for (auto& thread : threads) {
            thread.join();
        }

        // wait to see if there's a final exception, throw if so..
        {
            std::lock_guard<std::mutex> g(m);
            if (exc) std::rethrow_exception(exc);
        }
    }

    template <typename F, typename... Args>
    void ProduceWith(F&& f, Args&&... args)
    {
        using namespace std::placeholders;  // for _1, _2, _3...

        // Create a function taking Index, which delegates to
        // a function taking Index followed by args.
        TPTask task{std::bind(std::forward<F>(f), _1, std::forward<Args>(args)...)};

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
            head.emplace(boost::none);
        }
        pushed.notify_all();
    }

private:
    using TTask = boost::optional<TPTask>;

    TTask PopTask()
    {
        TTask task(boost::none);

        {
            std::unique_lock<std::mutex> lk(m);
            pushed.wait(lk, [&task, this]() {
                if (head.empty()) return false;

                if ((task = std::move(head.front()))) {
                    head.pop();
                    task->get_future();
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
};

}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H
