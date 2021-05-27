#ifndef PBCOPPER_LOGGING_LOGFIELD_H
#define PBCOPPER_LOGGING_LOGFIELD_H

namespace PacBio {
namespace Logging {
namespace LogField {

constexpr int NONE = 0x00;
constexpr int TIMESTAMP = 0x01;
constexpr int LOG_LEVEL = 0x02;
constexpr int FUNCTION = 0x04;
constexpr int THREAD_ID = 0x08;

constexpr int DEFAULT = TIMESTAMP | LOG_LEVEL | FUNCTION | THREAD_ID;

}  // namespace LogField
}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGFIELD_H
