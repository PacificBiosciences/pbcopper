#include <pbcopper/parallel/FireAndForget.h>

namespace PacBio {
namespace Parallel {
void Dispatch(Parallel::FireAndForget* const faf, const int32_t numEntries,
              const std::function<void(int32_t)>& callback)
{
    int32_t fafCounter{0};
    std::condition_variable condVar;
    std::mutex m;
    if (faf) {
        const auto Submit = [&](int32_t i) {
            callback(i);
            int32_t curCounter;
            {
                std::unique_lock<std::mutex> lock{m};
                ++fafCounter;
                curCounter = fafCounter;
            }
            if (curCounter == numEntries) {
                condVar.notify_one();
            }
        };

        for (int32_t i = 0; i < numEntries; ++i) {
            faf->ProduceWith(Submit, i);
        }

        {
            std::unique_lock<std::mutex> lock{m};
            condVar.wait(lock, [&fafCounter, numEntries] { return fafCounter == numEntries; });
        }
    } else {
        for (int32_t i = 0; i < numEntries; ++i) {
            callback(i);
        }
    }
}
}  // namespace Parallel
}  // namespace PacBio
