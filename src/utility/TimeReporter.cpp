#include <pbcopper/utility/TimeReporter.h>

#include <cassert>

#include <pbcopper/logging/Logging.h>

namespace PacBio {
namespace Utility {

TimeReporter::TimeReporter(const Logging::LogLevel lvl) : logLevel_{lvl}
{
    // simple default
    callback_ = [](const double nsec) {
        return "Finished in " + Utility::Stopwatch::PrettyPrintNanoseconds(nsec);
    };
}

TimeReporter::TimeReporter(const Logging::LogLevel lvl, TimeReporterCallback callback)
    : logLevel_{lvl}, callback_{callback}
{
}

TimeReporter::~TimeReporter()
{
    if (logLevel_ < Logging::CurrentLogLevel()) {
        return;
    }

    const std::string message = callback_(stopwatch_.ElapsedNanoseconds());
    switch (logLevel_) {
        case Logging::LogLevel::TRACE:
            PBLOG_TRACE << message;
            break;
        case Logging::LogLevel::DEBUG:
            PBLOG_DEBUG << message;
            break;
        case Logging::LogLevel::VERBOSE:
            PBLOG_VERBOSE << message;
            break;
        case Logging::LogLevel::INFO:
            PBLOG_INFO << message;
            break;
        case Logging::LogLevel::NOTICE:
            PBLOG_NOTICE << message;
            break;
        case Logging::LogLevel::WARN:
            PBLOG_WARN << message;
            break;
        case Logging::LogLevel::ERROR:
            PBLOG_ERROR << message;
            break;
        case Logging::LogLevel::CRITICAL:
            PBLOG_CRITICAL << message;
            break;
        case Logging::LogLevel::FATAL:
            PBLOG_FATAL << message;
            break;
        default:
            // can't throw from dtor
            assert(false);
    }
}

}  // namespace Utility
}  // namespace PacBio
