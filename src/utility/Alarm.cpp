#include <pbcopper/utility/Alarm.h>

#include <pbcopper/json/JSON.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>
#include <ostream>
#include <utility>

namespace PacBio {
namespace Utility {
namespace {

PacBio::JSON::Json ToJson(const Alarm& a)
{
    auto result = PacBio::JSON::Json::object();
    result["name"] = a.Name();
    result["message"] = a.Message();
    result["id"] = a.Id();
    result["exception"] = a.Exception();
    result["severity"] = a.Severity();
    result["owner"] = a.Owner();
    return result;
}

}  // namespace

Alarm::Alarm(std::string name, std::string message)
    : Alarm{std::move(name), std::move(message), "ERROR", "", ""}
{}

Alarm::Alarm(std::string name, std::string message, std::string severity, std::string info,
             std::string exception)
    : message_{std::move(message)}
    , name_{std::move(name)}
    , exception_{std::move(exception)}
    , info_{std::move(info)}
    , severity_{std::move(severity)}
{
    static boost::uuids::random_generator gen;
    const boost::uuids::uuid uuid = gen();
    uuid_ = boost::uuids::to_string(uuid);
}

Alarm::Alarm(const AlarmException& e)
    : Alarm{e.Name(), e.Message(), e.Severity(), e.Info(), e.Exception()}
{}

const std::string& Alarm::Exception() const { return exception_; }

Alarm& Alarm::Exception(std::string exception)
{
    exception_ = std::move(exception);
    return *this;
}

const std::string& Alarm::Id() const { return uuid_; }

Alarm& Alarm::Id(std::string uuid)
{
    uuid_ = std::move(uuid);
    return *this;
}

const std::string& Alarm::Info() const { return info_; }

Alarm& Alarm::Info(std::string info)
{
    info_ = std::move(info);
    return *this;
}

const std::string& Alarm::Message() const { return message_; }

Alarm& Alarm::Message(std::string message)
{
    message_ = std::move(message);
    return *this;
}

const std::string& Alarm::Name() const { return name_; }

Alarm& Alarm::Name(std::string name)
{
    name_ = std::move(name);
    return *this;
}

const std::string& Alarm::Owner() const { return owner_; }

Alarm& Alarm::Owner(std::string owner)
{
    owner_ = std::move(owner);
    return *this;
}

const std::string& Alarm::Severity() const { return severity_; }

Alarm& Alarm::Severity(std::string severity)
{
    severity_ = std::move(severity);
    return *this;
}

void Alarm::Print(const std::string& fn) const
{
    std::ofstream f{fn};
    Print(f);
}

void Alarm::Print(std::ostream& out) const
{
    const auto result = ToJson(*this);
    out << result.dump(4);
}

void Alarm::WriteAlarms(const std::string& fn, const std::vector<Alarm>& alarms,
                        const std::string& applicationName)
{
    if (fn.empty()) {
        throw std::runtime_error{"[pbcopper] alarm ERROR: cannot write to empty alarm filename"};
    }

    std::ofstream f{fn};
    WriteAlarms(f, alarms, applicationName);
}

void Alarm::WriteAlarms(std::ostream& out, const std::vector<Alarm>& alarms,
                        const std::string& applicationName)
{
    auto jsonAlarms = PacBio::JSON::Json::array();
    for (const auto& alarm : alarms) {
        auto jsonAlarm = ToJson(alarm);
        if (alarm.Owner().empty() && !applicationName.empty()) {
            jsonAlarm["owner"] = applicationName;
        }
        jsonAlarms.emplace_back(std::move(jsonAlarm));
    }
    out << jsonAlarms.dump(4);
}

std::ostream& operator<<(std::ostream& out, const Alarm& alarm)
{
    alarm.Print(out);
    return out;
}

AlarmException::AlarmException(std::string sourceFilename, std::string functionName,
                               int32_t lineNumber, std::string name, std::string message,
                               std::string severity, std::string info,
                               std::string exception) noexcept
    : sourceFilename_{std::move(sourceFilename)}
    , functionName_{std::move(functionName)}
    , lineNumber_{lineNumber}
    , name_{std::move(name)}
    , message_{std::move(message)}
    , severity_{std::move(severity)}
    , info_{std::move(info)}
    , exception_{std::move(exception)}
{}

AlarmException::AlarmException(std::string sourceFilename, std::string functionName,
                               int32_t lineNumber, AlarmType type, std::string message,
                               std::string severity, std::string info,
                               std::string exception) noexcept
    : AlarmException{std::move(sourceFilename), std::move(functionName), lineNumber,
                     NameForAlarmType(type),    std::move(message),      std::move(severity),
                     std::move(info),           std::move(exception)}
{}

// Debugging Info
const char* AlarmException::SourceFilename() const noexcept { return sourceFilename_.c_str(); }

const char* AlarmException::FunctionName() const noexcept { return functionName_.c_str(); }

int32_t AlarmException::LineNumber() const noexcept { return lineNumber_; }

// Used by Alarms API
const char* AlarmException::Name() const noexcept { return name_.c_str(); }

const char* AlarmException::Message() const noexcept { return message_.c_str(); }

const char* AlarmException::Severity() const noexcept { return severity_.c_str(); }

const char* AlarmException::Info() const noexcept { return info_.c_str(); }

const char* AlarmException::Exception() const noexcept { return exception_.c_str(); }

std::string NameForAlarmType(const AlarmType type)
{
    const std::map<AlarmType, std::string> lookup{
        {AlarmType::BARCODE, "BarcodeError"},
        {AlarmType::CHEMISTRY_MODEL, "ChemistryModelError"},
        {AlarmType::CLI, "CommandLineArgsError"},
        {AlarmType::FILE_FORMAT, "FileFormatError"},
        {AlarmType::FILE_NOT_FOUND, "FileNotFoundError"},
    };

    const auto found = lookup.find(type);
    if (found == lookup.cend()) {
        return std::string{};
    } else {
        return found->second;
    }
}

}  // namespace Utility
}  // namespace PacBio
