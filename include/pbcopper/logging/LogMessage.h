#ifndef PBCOPPER_LOGGING_LOGMESSAGE_H
#define PBCOPPER_LOGGING_LOGMESSAGE_H

#include <memory>
#include <sstream>
#include <utility>

#include <pbcopper/logging/LogLevel.h>

namespace PacBio {
namespace Logging {

class Logger;

using LogLevelStream = std::pair<LogLevel, std::ostringstream>;

class LogMessage
{
public:
    LogMessage(const char* file, const char* function, unsigned int line, const LogLevel level,
               Logger& logger);
    ~LogMessage();

public:
    template <typename T>
    LogMessage& operator<<(const T& t)
    {
        if (ptr_) {
            ptr_->second << t;
        }
        return *this;
    }

private:
    std::unique_ptr<LogLevelStream> ptr_;
    Logger& logger_;
};

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGMESSAGE_H
