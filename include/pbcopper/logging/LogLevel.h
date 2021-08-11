#ifndef PBCOPPER_LOGGING_LOGLEVEL_H
#define PBCOPPER_LOGGING_LOGLEVEL_H

#include <string>

namespace PacBio {
namespace Logging {

class LogLevel
{
public:
    enum
    {
        TRACE = 0,
        DEBUG = 1,
        VERBOSE = 2,
        INFO = 3,
        NOTICE = 4,
        WARN = 5,
        ERROR = 6,
        CRITICAL = 7,
        FATAL = 8,

        MAX_LOG_LEVEL = 9,
    };

public:
    LogLevel(const int value);
    LogLevel(const std::string& value);

public:
    operator int() const noexcept;
    std::string ToString() const;

private:
    int value_;
};

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGLEVEL_H
