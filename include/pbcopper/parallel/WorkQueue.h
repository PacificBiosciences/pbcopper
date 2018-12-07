// Author: Lance Hepler

#ifndef PBCOPPER_PARALLEL_WORKQUEUE_H
#define PBCOPPER_PARALLEL_WORKQUEUE_H

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <exception>
#include <future>
#include <mutex>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Parallel {

template <typename T>
class WorkQueue
{
private:
    typedef boost::optional<std::packaged_task<T()>> TTask;
    typedef boost::optional<std::future<T>> TFuture;

public:
    WorkQueue(const size_t size, const size_t mul = 2) : exc{nullptr}, sz{size * mul}
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
                    popped.notify_all();
                }
            }));
        }
    }

    ~WorkQueue()
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
        std::packaged_task<T()> task{std::bind(std::forward<F>(f), std::forward<Args>(args)...)};

        {
            std::unique_lock<std::mutex> lk(m);
            popped.wait(lk, [&task, this]() {
                if (exc) std::rethrow_exception(exc);

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
                cont(std::forward<Args>(args)..., std::move(fut->get()));
            }

            return true;
        } catch (...) {
            {
                std::lock_guard<std::mutex> g(m);
                exc = std::current_exception();
            }
            popped.notify_all();
        }
        return false;
    }

    void Finalize()
    {
        {
            std::lock_guard<std::mutex> g(m);
            head.emplace_back(boost::none);
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
};
}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_WORKQUEUE_H
