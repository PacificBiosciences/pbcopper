#include <pbcopper/logging/LogConfig.h>

namespace PacBio {
namespace Logging {

LogConfig::LogConfig() : LogConfig{LogLevel::WARN} {}

LogConfig::LogConfig(const LogLevel level)
    : Level{level}, Header{">|> "}, Delimiter{" -|- "}, Fields{LogField::DEFAULT}
{}

}  // namespace Logging
}  // namespace PacBio
