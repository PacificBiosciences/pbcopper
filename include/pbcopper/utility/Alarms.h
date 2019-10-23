// Author: Nathaniel Echols

#ifndef PBCOPPER_ALARMS_H
#define PBCOPPER_ALARMS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/json/JSON.h>

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace PacBio {
namespace Utility {

class PacBioAlarm
{
public:
    explicit PacBioAlarm(std::string name, std::string message);

    explicit PacBioAlarm(std::string name, std::string message, std::string severity,
                         std::string info, std::string exception);

    const std::string& Message() const;
    PacBioAlarm& Message(std::string message);

    const std::string& Name() const;
    PacBioAlarm& Name(std::string name);

    const std::string& Exception() const;
    PacBioAlarm& Exception(std::string exception);

    const std::string& Info() const;
    PacBioAlarm& Info(std::string info);

    const std::string& Severity() const;
    PacBioAlarm& Severity(std::string severity);

    const std::string& Owner() const;
    PacBioAlarm& Owner(std::string owner);

    const std::string& Id() const;
    PacBioAlarm& Id(std::string uuid);

    // print json format
    void Print(const std::string& fn) const;
    void Print(std::ostream& out) const;

    static void WriteAlarms(const std::string& fn, std::vector<PacBioAlarm> alarms);

private:
    std::string message_;
    std::string name_;
    std::string exception_;
    std::string info_;
    std::string severity_;
    std::string owner_;
    std::string uuid_;

    PacBio::JSON::Json ToJson() const;
};

}  // namespace:Utility
}  // namespace:PacBio

#endif  // PBCOPPER_ALARMS_H
