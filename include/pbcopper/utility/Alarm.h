// Author: Nathaniel Echols, Derek Barnett

#ifndef PBCOPPER_UTILITY_ALARM_H
#define PBCOPPER_UTILITY_ALARM_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
#include <string>
#include <vector>

namespace PacBio {
namespace Utility {

class Alarm
{
public:
    static void WriteAlarms(const std::string& fn, const std::vector<Alarm>& alarms);
    static void WriteAlarms(std::ostream& out, const std::vector<Alarm>& alarms);

public:
    Alarm(std::string name, std::string message);
    Alarm(std::string name, std::string message, std::string severity, std::string info,
          std::string exception);

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
    void Print(const std::string& fn) const;
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

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_ALARM_H
