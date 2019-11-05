// Author: Nathaniel Echols, Derek Barnett

#include <pbcopper/utility/Alarm.h>

#include <fstream>
#include <iostream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pbcopper/json/JSON.h>

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
{
}

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

void Alarm::WriteAlarms(const std::string& fn, const std::vector<Alarm>& alarms)
{
    std::ofstream f{fn};
    WriteAlarms(f, alarms);
}

void Alarm::WriteAlarms(std::ostream& out, const std::vector<Alarm>& alarms)
{
    auto jsonAlarms = PacBio::JSON::Json::array();
    for (const auto& alarm : alarms)
        jsonAlarms.emplace_back(ToJson(alarm));
    out << jsonAlarms.dump(4);
}

std::ostream& operator<<(std::ostream& out, const Alarm& alarm)
{
    alarm.Print(out);
    return out;
}

}  // namespace Utility
}  // namespace PacBio
