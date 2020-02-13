// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_STOPWATCH_INL_H
#define PBCOPPER_UTILITY_STOPWATCH_INL_H

#include <pbcopper/utility/Stopwatch.h>

namespace PacBio {
namespace Utility {

inline Stopwatch::Stopwatch() { Reset(); }

template <typename TimeUnit>
double Stopwatch::Elapsed() const
{
    if (frozen_) {
        return std::chrono::duration_cast<TimeUnit>(tock_ - tick_).count();
    } else {
        auto tock = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<TimeUnit>(tock - tick_).count();
    }
}

inline double Stopwatch::ElapsedNanoseconds() const { return Elapsed<std::chrono::nanoseconds>(); }
inline double Stopwatch::ElapsedMilliseconds() const
{
    return Elapsed<std::chrono::milliseconds>();
}
inline double Stopwatch::ElapsedSeconds() const { return Elapsed<std::chrono::seconds>(); }

inline std::string Stopwatch::ElapsedTime() const
{
    auto nanosecs = ElapsedNanoseconds();
    return PrettyPrintNanoseconds(nanosecs);
}

inline void Stopwatch::Reset()
{
    tick_ = std::chrono::steady_clock::now();
    frozen_ = false;
}

inline void Stopwatch::Freeze()
{
    tock_ = std::chrono::steady_clock::now();
    frozen_ = true;
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_STOPWATCH_INL_H
