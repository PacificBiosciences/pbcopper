// Author: Lance Hepler, Derek Barnett

#ifndef PBCOPPER_LOGGING_LOGCONFIG_H
#define PBCOPPER_LOGGING_LOGCONFIG_H

#include <string>

#include <pbcopper/logging/LogField.h>
#include <pbcopper/logging/LogLevel.h>

namespace PacBio {
namespace Logging {

struct LogConfig
{
    LogConfig();
    explicit LogConfig(const LogLevel level);

    LogLevel Level;
    std::string Header;
    std::string Delimiter;
    int Fields;
};

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGCONFIG_H
