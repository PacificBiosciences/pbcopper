// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_STOPWATCH_H
#define PBCOPPER_UTILITY_STOPWATCH_H

#include <chrono>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

class Stopwatch
{
public:
    /// \name Constructors & Related Methods
    /// \{

    /// Creates a stopwatch and begins timing.
    Stopwatch();

    Stopwatch(const Stopwatch& other) = default;
    Stopwatch(Stopwatch&& other) = default;
    Stopwatch& operator=(const Stopwatch& other) = default;
    Stopwatch& operator=(Stopwatch& other) = default;
    ~Stopwatch() = default;

    /// \}

public:
    /// \returns the elapsed time (in milliseconds) since timing began.
    float ElapsedMilliseconds() const;

    /// \returns the elapsed time (in seconds) since timing began.
    float ElapsedSeconds() const;

    /// \returns the elapsed time (in user-provided units) since timing began.
    template <typename TimeUnit>
    float Elapsed() const;

    // resets internal values
    void Reset();

private:
    std::chrono::steady_clock::time_point start_;
};

}  // namespace Utility
}  // namespace PacBio

#include <pbcopper/utility/internal/Stopwatch-inl.h>

#endif  // PBCOPPER_UTILITY_STOPWATCH_H
