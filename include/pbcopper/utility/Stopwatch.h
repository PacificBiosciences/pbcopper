// Author: Derek Barnett
// Author: Armin Toepfer

#ifndef PBCOPPER_UTILITY_STOPWATCH_H
#define PBCOPPER_UTILITY_STOPWATCH_H

#include <pbcopper/PbcopperConfig.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <chrono>
#include <string>

namespace PacBio {
namespace Utility {

class Stopwatch
{
public:
    /// \name Constructors & Related Methods
    /// \{

    /// Creates a stopwatch and begins timing.
    Stopwatch();

    Stopwatch(const Stopwatch&) = default;
    Stopwatch(Stopwatch&&) noexcept = default;
    Stopwatch& operator=(const Stopwatch&) = default;
    Stopwatch& operator=(Stopwatch&&) noexcept = default;
    ~Stopwatch() = default;

    /// \}

public:
    /// \returns the elapsed time (in nanoseconds) between tick tock.
    float ElapsedNanoseconds() const;
    /// \returns the elapsed time (in milliseconds) between tick tock.
    float ElapsedMilliseconds() const;
    /// \returns the elapsed time (in seconds) between tick tock.
    float ElapsedSeconds() const;

    /// \returns the elapsed time (in user-provided units) between tick tock.
    template <typename TimeUnit>
    float Elapsed() const;

    /// \returns the elapsed time (in string) between tick tock.
    std::string ElapsedTime() const;

    /// \returns the elapsed time (in string) given input nanoseconds
    static std::string PrettyPrintNanoseconds(int64_t nanosecs);

    /// \returns the CPU time (in nanoseconds) between tick tock.
    static double CpuTime();

    /// Resets a stopwatch and begins timing.
    void Reset();

    /// Freeze a stopwatch and stops timing. This timer can be reset to time a new
    /// thread or process, but can *not* resume to get cumulative sum of multiple
    /// intermittent processes.
    void Freeze();

private:
    bool frozen_ = false;
    std::chrono::time_point<std::chrono::steady_clock> tick_;
    std::chrono::time_point<std::chrono::steady_clock> tock_;
};

}  // namespace Utility
}  // namespace PacBio

#include <pbcopper/utility/internal/Stopwatch-inl.h>

#endif  // PBCOPPER_UTILITY_STOPWATCH_H
