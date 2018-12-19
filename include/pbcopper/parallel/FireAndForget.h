// Author: Lance Hepler & Armin Töpfer

#ifndef PBCOPPER_PARALLEL_FIREANDFORGET_H
#define PBCOPPER_PARALLEL_FIREANDFORGET_H

#include <condition_variable>
#include <cstddef>
#include <exception>
#include <future>
#include <mutex>
#include <queue>

#include <boost/optional.hpp>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Parallel {

class FireAndForget
{
private:
    typedef boost::optional<std::packaged_task<void()>> TTask;

public:
    FireAndForget(const size_t size, const size_t mul = 2) : exc{nullptr}, sz{size * mul}
    {
        for (size_t i = 0; i < size; ++i) {
            threads.emplace_back(std::thread([this]() {
                try {
                    while (auto task = PopTask()) {
                        (*task)();
                    }
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

    ~FireAndForget()
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
            head.emplace(boost::none);
        }
        pushed.notify_all();
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

#endif  // PBCOPPER_PARALLEL_FIREANDFORGET_H
