// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_TIMEREPORTER_H
#define PBCOPPER_UTILITY_TIMEREPORTER_H

#include <pbcopper/PbcopperConfig.h>

#include <functional>
#include <string>

#include <pbcopper/logging/LogLevel.h>
#include <pbcopper/utility/Stopwatch.h>

namespace PacBio {
namespace Utility {

///
/// Callable that should take the nanoseconds from Stopwatch and return the
/// desired output message for the task. "Task-local" variables can be used in
/// the message via lambda capture.
///
using TimeReporterCallback = std::function<std::string(double)>;

class TimeReporter
{
public:
    explicit TimeReporter(Logging::LogLevel logLevel);
    TimeReporter(Logging::LogLevel logLevel, TimeReporterCallback callback);
    ~TimeReporter();

private:
    Logging::LogLevel logLevel_;
    TimeReporterCallback callback_;
    Utility::Stopwatch stopwatch_;
};

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_TIMEREPORTER_H
