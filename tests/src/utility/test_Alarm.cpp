// Author: Nathaniel Echols, Derek Barnett

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include <pbcopper/utility/Alarm.h>

// clang-format off
TEST(Utility_Alarm, can_write_single_alarm)
{
    const std::string defaultUuid{"44af40b0-2986-401e-b21a-7051b40189fb"};
    const std::string expectedText{
R"({
    "exception": "This is an exception field",
    "id": "44af40b0-2986-401e-b21a-7051b40189fb",
    "message": "Hello, world!",
    "name": "test_alarm",
    "owner": "",
    "severity": "ERROR"
})"};
    PacBio::Utility::Alarm alarm{"test_alarm", "Hello, world!", "ERROR", "This is an info field",
                                 "This is an exception field"};
    std::ostringstream s;
    alarm.Id(defaultUuid).Print(s);
    EXPECT_EQ(expectedText, s.str());

    s.str("");
    s << alarm;
    EXPECT_EQ(expectedText, s.str());
}

TEST(Utility_Alarm, can_write_multiple_alarms)
{
    PacBio::Utility::Alarm alarm1{"test_alarm1", "Hello world!"};
    PacBio::Utility::Alarm alarm2{"test_alarm2", "Hello again!"};

    std::ostringstream s;
    PacBio::Utility::Alarm::WriteAlarms(s, {alarm1, alarm2});

    EXPECT_NE(s.str().find("test_alarm1"), std::string::npos);
    EXPECT_NE(s.str().find("test_alarm2"), std::string::npos);
}

// clang-format on