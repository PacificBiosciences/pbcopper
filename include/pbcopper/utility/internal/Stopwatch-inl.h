#ifndef PBCOPPER_UTILITY_STOPWATCH_INL_H
#define PBCOPPER_UTILITY_STOPWATCH_INL_H

#include "pbcopper/utility/Stopwatch.h"

namespace PacBio {
namespace Utility {

inline Stopwatch::Stopwatch(void)
{
    Reset();
}

template<typename TimeUnit>
float Stopwatch::Elapsed(void) const
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<TimeUnit>(now - start_).count();
}

inline float Stopwatch::ElapsedMilliseconds(void) const
{
    return Elapsed<std::chrono::milliseconds>();
}

inline float Stopwatch::ElapsedSeconds(void) const
{
    return Elapsed<std::chrono::seconds>();
}

void Stopwatch::Reset(void)
{
    start_ = std::chrono::steady_clock::now();
}

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_STOPWATCH_INL_H
