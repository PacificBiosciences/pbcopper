// Author: Nathaniel Echols

#include <pbcopper/utility/Alarms.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>
#include <iostream>

using Json = PacBio::JSON::Json;

namespace PacBio {
namespace Utility {

PacBioAlarm::PacBioAlarm(std::string name, std::string message)
    : PacBioAlarm{name, message, "ERROR", "", ""}
{
}

PacBioAlarm::PacBioAlarm(std::string name, std::string message, std::string severity,
                         std::string info, std::string exception)
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

const std::string& PacBioAlarm::Message() const { return message_; }
const std::string& PacBioAlarm::Name() const { return name_; }
const std::string& PacBioAlarm::Info() const { return info_; }
const std::string& PacBioAlarm::Severity() const { return severity_; }
const std::string& PacBioAlarm::Exception() const { return exception_; }
const std::string& PacBioAlarm::Owner() const { return owner_; }
const std::string& PacBioAlarm::Id() const { return uuid_; }

PacBioAlarm& PacBioAlarm::Message(std::string message)
{
    message_ = std::move(message);
    return *this;
}

PacBioAlarm& PacBioAlarm::Name(std::string name)
{
    name_ = std::move(name);
    return *this;
}

PacBioAlarm& PacBioAlarm::Exception(std::string exception)
{
    exception_ = std::move(exception);
    return *this;
}

PacBioAlarm& PacBioAlarm::Info(std::string info)
{
    info_ = std::move(info);
    return *this;
}

PacBioAlarm& PacBioAlarm::Severity(std::string severity)
{
    severity_ = std::move(severity);
    return *this;
}

PacBioAlarm& PacBioAlarm::Owner(std::string owner)
{
    owner_ = std::move(owner);
    return *this;
}

PacBioAlarm& PacBioAlarm::Id(std::string uuid)
{
    uuid_ = std::move(uuid);
    return *this;
}

void PacBioAlarm::Print(const std::string& fn) const
{
    std::ofstream f{fn};
    Print(f);
}

void PacBioAlarm::Print(std::ostream& out) const
{
    Json result = ToJson();
    out << result.dump(4);
}

Json PacBioAlarm::ToJson() const
{
    Json result = Json::object();
    result["name"] = Name();
    result["message"] = Message();
    result["id"] = Id();
    result["exception"] = Exception();
    result["severity"] = Severity();
    result["owner"] = Owner();
    return result;
}

void PacBioAlarm::WriteAlarms(const std::string& fn, std::vector<PacBioAlarm> alarms)
{
    Json jsonAlarms = Json::array();
    for (const auto& alarm : alarms)
        jsonAlarms.emplace_back(alarm.ToJson());
    std::ofstream out{fn};
    out << jsonAlarms.dump(4);
}

}  // namespace:Utility
}  // namespace:PacBio
