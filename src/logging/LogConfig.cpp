// Author: Derek Barnett

#include <pbcopper/logging/LogConfig.h>

namespace PacBio {
namespace Logging {

LogConfig::LogConfig() : LogConfig{LogLevel::INFO} {}

LogConfig::LogConfig(const LogLevel level)
    : Level{level}
    , Header{">|> "}
    , Delimiter{" -|- "}  // , TimestampFormat{"%Y%m%d %T."}
    , Fields{LogField::DEFAULT}
{
}

}  // namespace Logging
}  // namespace PacBio