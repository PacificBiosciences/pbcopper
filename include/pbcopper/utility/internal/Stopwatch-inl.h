// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_STOPWATCH_INL_H
#define PBCOPPER_UTILITY_STOPWATCH_INL_H

#include <pbcopper/utility/Stopwatch.h>

namespace PacBio {
namespace Utility {

inline Stopwatch::Stopwatch() { Reset(); }

template <typename TimeUnit>
float Stopwatch::Elapsed() const
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<TimeUnit>(now - start_).count();
}

inline float Stopwatch::ElapsedMilliseconds() const { return Elapsed<std::chrono::milliseconds>(); }

inline float Stopwatch::ElapsedSeconds() const { return Elapsed<std::chrono::seconds>(); }

void Stopwatch::Reset() { start_ = std::chrono::steady_clock::now(); }

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_STOPWATCH_INL_H
