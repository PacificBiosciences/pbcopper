#include <pbcopper/parallel/FireAndForget.h>

#include <exception>

namespace PacBio {
namespace Parallel {
void Dispatch(Parallel::FireAndForget* const faf, const std::int32_t numEntries,
              const std::function<void(std::int32_t)>& callback)
{
    // Dispatch to FireAndForget
    if (faf) {
        // Double-checked locking to catch first exception only
        std::once_flag exceptionOnceFlag;
        // Exception to be stored and later rethrown
        std::exception_ptr exc;
        // Flag to abort all threads
        bool abort{false};
        // Counter to check if all jobs finished
        std::int32_t jobsFinished{0};
        // Mutex and condition variable to wait for all threads to finish
        std::condition_variable condVar;
        std::mutex m;

        // Set exception once and notify about it
        const auto SetFirstException = [&]() {
            std::call_once(exceptionOnceFlag, [&]() {
                exc = std::current_exception();
                abort = true;
            });
        };

        // Submit callback and handle finalization
        const auto Submit = [&](std::int32_t i) {
            try {
                callback(i);
            } catch (...) {
                SetFirstException();
            }

            // If this is the last entry or something failed, notify about it
            std::scoped_lock<std::mutex> lock{m};
            ++jobsFinished;
            if (jobsFinished == numEntries) {
                condVar.notify_one();
            }
        };

        // Dispatch all callbacks
        for (std::int32_t i = 0; i < numEntries; ++i) {
            // Submit callback and handle exceptions
            try {
                faf->ProduceWith(Submit, i);
            } catch (...) {
                SetFirstException();
            }
        }

        // Wait for all threads to finish or abort if something failed
        std::unique_lock<std::mutex> lock{m};
        condVar.wait(lock, [&] { return (jobsFinished == numEntries); });

        // If something failed, rethrow exception
        if (abort) {
            std::rethrow_exception(exc);
        }
    } else {
        // If no FaF is provided, just run the callback directly
        for (std::int32_t i = 0; i < numEntries; ++i) {
            callback(i);
        }
    }
}
}  // namespace Parallel
}  // namespace PacBio
