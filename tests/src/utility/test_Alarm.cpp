#include <pbcopper/utility/Alarm.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace AlarmTests {

const std::string TestUuid{"44af40b0-2986-401e-b21a-7051b40189fb"};

}  // namespace AlarmTests

// clang-format off
TEST(Utility_Alarm, can_write_single_alarm)
{
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
    alarm.Id(AlarmTests::TestUuid);

    std::ostringstream s;
    alarm.Print(s);
    EXPECT_EQ(expectedText, s.str());

    s.str("");
    s << alarm;
    EXPECT_EQ(expectedText, s.str());
}
// clang-format on

TEST(Utility_Alarm, can_write_multiple_alarms)
{
    const PacBio::Utility::Alarm alarm1{"test_alarm1", "Hello world!"};
    const PacBio::Utility::Alarm alarm2{"test_alarm2", "Hello again!"};

    std::ostringstream s;
    PacBio::Utility::Alarm::WriteAlarms(s, {alarm1, alarm2});

    EXPECT_NE(s.str().find("test_alarm1"), std::string::npos);
    EXPECT_NE(s.str().find("test_alarm2"), std::string::npos);
}

// clang-format off
TEST(Utility_Alarm, can_throw_typed_alarm_exceptions)
{
    try {
        throw PB_BARCODE_ALARM("invalid barcode label: 'foo'");
    } catch (const PacBio::Utility::AlarmException& e) {
        const std::string expectedText{
R"([
    {
        "exception": "",
        "id": "44af40b0-2986-401e-b21a-7051b40189fb",
        "message": "invalid barcode label: 'foo'",
        "name": "BarcodeError",
        "owner": "",
        "severity": "FATAL"
    }
]
)"};
        std::ostringstream s;
        PacBio::Utility::Alarm alarm{e};
        alarm.Id(AlarmTests::TestUuid);

        PacBio::Utility::Alarm::WriteAlarms(s, {alarm});
        EXPECT_EQ(s.str(), expectedText);
    }
}

TEST(Utility_Alarm, can_throw_generic_alarm_exceptions)
{
    try {
        throw PB_ALARM("Invalid data", "record_123 is missing 'sn' tag");
    } catch (const PacBio::Utility::AlarmException& e) {
        const std::string expectedText{
R"([
    {
        "exception": "",
        "id": "44af40b0-2986-401e-b21a-7051b40189fb",
        "message": "record_123 is missing 'sn' tag",
        "name": "Invalid data",
        "owner": "",
        "severity": "FATAL"
    }
]
)"};
        std::ostringstream s;
        PacBio::Utility::Alarm alarm{e};
        alarm.Id(AlarmTests::TestUuid);

        PacBio::Utility::Alarm::WriteAlarms(s, {alarm});
        EXPECT_EQ(s.str(), expectedText);
    }
}

TEST(Utility_Alarm, throws_on_empty_alarm_filename)
{
    const std::vector<PacBio::Utility::Alarm> alarms {
        PacBio::Utility::Alarm{"test_alarm1", "Hello world!"},
        PacBio::Utility::Alarm{"test_alarm2", "Hello again!"},
    };

    std::string fn;
    EXPECT_THROW(PacBio::Utility::Alarm::WriteAlarms(fn, alarms), std::runtime_error);
}

// clang-format on
