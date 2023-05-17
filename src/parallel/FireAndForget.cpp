#include <pbcopper/parallel/FireAndForget.h>

#include <atomic>
#include <exception>

namespace PacBio {
namespace Parallel {
void Dispatch(Parallel::FireAndForget* const faf, const std::int32_t numEntries,
              const std::function<void(std::int32_t)>& callback)
{
    if (faf) {
        std::exception_ptr exc;
        std::atomic_bool abort{false};

        std::int32_t fafCounter{0};
        std::condition_variable condVar;
        std::mutex m;
        const auto Submit = [&](std::int32_t i) {
            try {
                callback(i);
            } catch (...) {
                std::unique_lock<std::mutex> lock{m};
                if (!abort) {
                    exc = std::current_exception();
                    abort = true;
                }
            }

            std::unique_lock<std::mutex> lock{m};
            if ((++fafCounter == numEntries) || abort) {
                condVar.notify_one();
            }
        };

        for (std::int32_t i = 0; i < numEntries; ++i) {
            faf->ProduceWith(Submit, i);
        }

        {
            std::unique_lock<std::mutex> lock{m};
            condVar.wait(lock, [&fafCounter, numEntries, &abort] {
                return (fafCounter == numEntries) || abort;
            });
        }

        if (abort) {
            std::rethrow_exception(exc);
        }
    } else {
        for (std::int32_t i = 0; i < numEntries; ++i) {
            callback(i);
        }
    }
}
}  // namespace Parallel
}  // namespace PacBio
