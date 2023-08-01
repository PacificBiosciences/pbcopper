#ifndef PBCOPPER_LOGGING_LOGCONFIG_H
#define PBCOPPER_LOGGING_LOGCONFIG_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/logging/LogField.h>
#include <pbcopper/logging/LogLevel.h>

#include <string>

#include <cstdint>

namespace PacBio {
namespace Logging {

struct LogConfig
{
    LogConfig();
    explicit LogConfig(LogLevel level);

    LogLevel Level;
    std::string Header;
    std::string Delimiter;
    int Fields;
    std::int32_t LeftBlockWidth;
    bool AlignLevel;
};

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGCONFIG_H
