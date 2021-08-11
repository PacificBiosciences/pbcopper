#include <pbcopper/parallel/FireAndForget.h>

namespace PacBio {
namespace Parallel {
void Dispatch(Parallel::FireAndForget* const faf, const int32_t numEntries,
              const std::function<void(int32_t)>& callback)
{
    if (faf) {
        int32_t fafCounter{0};
        std::condition_variable condVar;
        std::mutex m;
        const auto Submit = [&](int32_t i) {
            callback(i);
            {
                std::unique_lock<std::mutex> lock{m};
                if (++fafCounter == numEntries) {
                    condVar.notify_one();
                }
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
