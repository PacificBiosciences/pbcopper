#include <pbcopper/logging/LogLevel.h>

#include <stdexcept>

namespace PacBio {
namespace Logging {
namespace {

LogLevel LogLevelFromString(const std::string& level)
{
    if (level == "TRACE") {
        return LogLevel::TRACE;
    }
    if (level == "DEBUG") {
        return LogLevel::DEBUG;
    }
    if (level == "VERBOSE") {
        return LogLevel::VERBOSE;
    }
    if (level == "INFO") {
        return LogLevel::INFO;
    }
    if (level == "NOTICE") {
        return LogLevel::NOTICE;
    }
    if (level == "ERROR") {
        return LogLevel::ERROR;
    }
    if (level == "CRITICAL") {
        return LogLevel::CRITICAL;
    }
    if (level == "FATAL") {
        return LogLevel::FATAL;
    }
    if (level == "WARN" || level == "WARNING") {
        return LogLevel::WARN;
    }

    throw std::invalid_argument{"[pbcopper] logging ERROR: invalid log level '" + level + "'"};
}

}  // namespace

LogLevel::LogLevel(const int value) : value_{value} {}

LogLevel::LogLevel(const std::string& value) : value_{LogLevelFromString(value)} {}

std::string LogLevel::ToString() const
{
    switch (value_) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::VERBOSE:
            return "VERBOSE";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::NOTICE:
            return "NOTICE";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::CRITICAL:
            return "CRITICAL";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "OTHER";
    }
}

LogLevel::operator int() const noexcept { return value_; }

}  // namespace Logging
}  // namespace PacBio
