#include <pbcopper/parallel/ThreadCount.h>

#include <algorithm>
#include <thread>

namespace PacBio {
namespace Parallel {

unsigned int NormalizedThreadCount(unsigned int requestedNumThreads)
{
    // Will be 0 if unknown
    const auto maxNumThreads = std::thread::hardware_concurrency();

    // "auto-detect" requested
    if (requestedNumThreads == 0) {
        return std::max(1U, maxNumThreads);
    }

    // actual maximum is unknown, just use the requested count
    if (maxNumThreads == 0) {
        return requestedNumThreads;
    }

    return std::min(requestedNumThreads, maxNumThreads);
}

}  // namespace Parallel
}  // namespace PacBio
