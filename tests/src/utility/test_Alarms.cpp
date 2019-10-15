// Author: Nathaniel Echols

#include <gtest/gtest.h>

#include <pbcopper/utility/Alarms.h>

#include <sstream>
#include <string>
#include <vector>

#include <pbcopper/utility/PbcopperVersion.h>

using namespace PacBio;
using namespace PacBio::Utility;

TEST(Alarms_Alarm, can_print_alarm)
{
    const std::string defaultUuid{"44af40b0-2986-401e-b21a-7051b40189fb"};
    const std::string expectedText{R"({
    "exception": "This is an exception field",
    "id": "44af40b0-2986-401e-b21a-7051b40189fb",
    "message": "Hello, world!",
    "name": "test_alarm",
    "owner": "",
    "severity": "ERROR"
})"};
    PacBioAlarm alarm{"test_alarm", "Hello, world!", "ERROR", "This is an info field",
                      "This is an exception field"};
    std::ostringstream s;
    alarm.Id(defaultUuid).Print(s);
    EXPECT_EQ(expectedText, s.str());
}

TEST(Alarms_Alarm, can_write_alarm_file)
{
    PacBioAlarm("test_alarm", "Hello world!").Print("alarm.json");
}

TEST(Alarms_Alarm, can_write_alarms_file)
{
    PacBioAlarm alarm1{"test_alarm1", "Hello world!"};
    PacBioAlarm alarm2{"test_alarm2", "Hello again!"};
    PacBioAlarm::WriteAlarms("alarms.json", std::vector<PacBioAlarm>{alarm1, alarm2});
}
