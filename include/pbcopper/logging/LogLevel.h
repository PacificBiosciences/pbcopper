// Author: Lance Hepler, Derek Barnett

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
        INFO = 2,
        NOTICE = 3,
        WARN = 4,
        ERROR = 5,
        CRITICAL = 6,
        FATAL = 7,

        MAX_LOG_LEVEL = 8
    };

public:
    LogLevel(const int value);
    LogLevel(const std::string& value);

public:
    operator int() const;
    std::string ToString() const;

private:
    int value_;
};

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGLEVEL_H