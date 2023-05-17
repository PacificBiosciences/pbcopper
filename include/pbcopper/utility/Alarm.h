#ifndef PBCOPPER_UTILITY_ALARM_H
#define PBCOPPER_UTILITY_ALARM_H

#include <pbcopper/PbcopperConfig.h>

#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

#include <cstdint>

namespace PacBio {
namespace Utility {

class AlarmException;

class Alarm
{
public:
    static void WriteAlarms(const std::filesystem::path& filename, const std::vector<Alarm>& alarms,
                            const std::string& applicationName = "");
    static void WriteAlarms(std::ostream& out, const std::vector<Alarm>& alarms,
                            const std::string& applicationName = "");

public:
    Alarm(std::string name, std::string message);
    Alarm(std::string name, std::string message, std::string severity, std::string info,
          std::string exception);
    Alarm(const AlarmException& alarmException);

    const std::string& Exception() const;
    Alarm& Exception(std::string exception);

    const std::string& Id() const;
    Alarm& Id(std::string uuid);

    const std::string& Info() const;
    Alarm& Info(std::string info);

    const std::string& Message() const;
    Alarm& Message(std::string message);

    const std::string& Name() const;
    Alarm& Name(std::string name);

    const std::string& Owner() const;
    Alarm& Owner(std::string owner);

    const std::string& Severity() const;
    Alarm& Severity(std::string severity);

    // print json format
    void Print(const std::filesystem::path& filename) const;
    void Print(std::ostream& out) const;

private:
    std::string message_;
    std::string name_;
    std::string exception_;
    std::string info_;
    std::string severity_;
    std::string owner_;
    std::string uuid_;
};

std::ostream& operator<<(std::ostream& out, const Alarm& alarm);

enum class AlarmType
{
    BARCODE,
    CHEMISTRY_MODEL,
    CLI,
    FILE_FORMAT,
    FILE_NOT_FOUND,
};

std::string NameForAlarmType(AlarmType type);

// AlarmException does not inherit from std::exception on purpose:
// The idea is that AlarmException is orthogonal to the standard C++
// exception hierarchy in order to bypass the standard
// try { ... } catch(const std::exception&) { ... }
// blocks, such that only CLIv2 will eventually catch it.
class AlarmException
{
public:
    AlarmException(std::string sourceFilename, std::string functionName, std::int32_t lineNumber,
                   std::string name, std::string message, std::string severity, std::string info,
                   std::string exception) noexcept;

    AlarmException(std::string sourceFilename, std::string functionName, std::int32_t lineNumber,
                   AlarmType type, std::string message, std::string severity, std::string info,
                   std::string exception) noexcept;

    // Debugging Info
    const char* SourceFilename() const noexcept;
    const char* FunctionName() const noexcept;
    std::int32_t LineNumber() const noexcept;

    // Used by Alarms API
    const char* Name() const noexcept;  // this is the error type sent to TS
    const char* Message() const noexcept;
    const char* Severity() const noexcept;
    const char* Info() const noexcept;
    const char* Exception() const noexcept;

protected:
    std::string sourceFilename_;
    std::string functionName_;
    std::int32_t lineNumber_;

    std::string name_;
    std::string message_;
    std::string severity_;
    std::string info_;
    std::string exception_;
};

}  // namespace Utility
}  // namespace PacBio

#define PB_ALARM_EXCEPTION_IMPL(name, message, severity, info, exception)                        \
    PacBio::Utility::AlarmException(__FILE__, __func__, __LINE__, name, message, severity, info, \
                                    exception)

// -----------------------------------
// Built-in typed alarms
// -----------------------------------

#define PB_TYPED_ALARM_IMPL(type, message) PB_ALARM_EXCEPTION_IMPL(type, message, "FATAL", "", "")

///
/// Application-specific, or otherwise not covered by built-ins below.
/// 'type' should be a general category of error, with 'message' containing
/// the usual detailed information provided in a thrown exception
///
#define PB_ALARM(type, message) PB_TYPED_ALARM_IMPL(type, message)

///
/// Missing barcode tags, invalid labels, ...
///
#define PB_BARCODE_ALARM(message) PB_TYPED_ALARM_IMPL(PacBio::Utility::AlarmType::BARCODE, message)

///
/// Invalid part number combination, error loading model file, ...
///
#define PB_CHEMISTRY_ALARM(message) \
    PB_TYPED_ALARM_IMPL(PacBio::Utility::AlarmType::CHEMISTRY_MODEL, message)

///
/// Missing filename(s), mutually-exclusive options, out-of-range value, ...
///
#define PB_CLI_ALARM(message) PB_TYPED_ALARM_IMPL(PacBio::Utility::AlarmType::CLI, message)

///
/// Unsupported file format
///
#define PB_FILE_FORMAT_ALARM(message) \
    PB_TYPED_ALARM_IMPL(PacBio::Utility::AlarmType::FILE_FORMAT, message)

///
/// File not found
///
#define PB_FILE_NOT_FOUND_ALARM(message) \
    PB_TYPED_ALARM_IMPL(PacBio::Utility::AlarmType::FILE_NOT_FOUND, message)

#endif  // PBCOPPER_UTILITY_ALARM_H
