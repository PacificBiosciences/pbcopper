#include <pbcopper/reports/Report.h>

#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/json/JSON.h>
#include <pbcopper/utility/PbcopperVersion.h>

using namespace PacBio;
using namespace PacBio::Reports;

namespace ReportsTests {

Report MakeReport()
{
    const int foo_value = 3;
    const std::string bar_value = "test";
    const double baz_value = 0.75;

    Report report{"isoseq3_flnca", "Report isoseq3"};
    report.Attributes({{"foo", foo_value, "Foo Title"},
                       {"bar", bar_value, "Bar Title"},
                       {"baz", baz_value, "Baz Title"}});
    report.Uuid("testable_uuid");

    Table table{"table1"};
    table.AddColumn(Column{"col1", std::vector<ReportValue>{1, 2, 3, 4}});
    report.AddTable(table);

    return report;
}

}  // namespace ReportsTests

// clang-format off
TEST(Reports_Report, can_print_pbreport)
{
    std::string expectedText{R"({
    "_comment": "Created by pbcopper v____",
    "attributes": [
        {
            "id": "foo",
            "name": "Foo Title",
            "value": 3
        },
        {
            "id": "bar",
            "name": "Bar Title",
            "value": "test"
        },
        {
            "id": "baz",
            "name": "Baz Title",
            "value": 0.75
        }
    ],
    "dataset_uuids": [],
    "id": "isoseq3_flnca",
    "plotGroups": [],
    "tables": [
        {
            "columns": [
                {
                    "header": "",
                    "id": "col1",
                    "values": [
                        1,
                        2,
                        3,
                        4
                    ]
                }
            ],
            "id": "table1",
            "title": ""
        }
    ],
    "title": "Report isoseq3",
    "uuid": "testable_uuid",
    "version": "1.0.1"
})"};
    expectedText.replace(40, 4, PacBio::Utility::LibraryVersionString());

    std::ostringstream s;
    const auto report = ReportsTests::MakeReport();
    report.Print(s);
    EXPECT_EQ(expectedText, s.str());
}

TEST(Reports_Report, can_print_summary)
{
    const std::string expectedText{R"({
    "bar": "test",
    "baz": 0.75,
    "foo": 3
})"};

    std::ostringstream s;
    const auto report = ReportsTests::MakeReport();
    report.PrintSummary(s);
    EXPECT_EQ(expectedText, s.str());
}
// clang-format on

TEST(Reports_Report, can_write_task_report)
{
    const TaskReport report{16, 100, 0, 4096};
    std::ostringstream s;
    report.Print(s);

    EXPECT_NE(s.str().find("Task Report"), std::string::npos);
    EXPECT_NE(s.str().find("Number of cores/slots"), std::string::npos);
    EXPECT_NE(s.str().find("Run time (seconds)"), std::string::npos);
    EXPECT_NE(s.str().find("Exit code"), std::string::npos);
    EXPECT_NE(s.str().find("Peak RSS (GB)"), std::string::npos);
}

TEST(Reports_Report, can_report_longints)
{
    EXPECT_EQ(0, 0);
    Report report{"isoseq3_flnca", "Report isoseq3"};
    // clang-format off
    report.Attributes({
        {"foo", 0xffffffffu, "Foo Title for uint32_t"},
        {"bar", static_cast<int64_t>(0xffffffffffff), "Bar Title for long long"},
        {"bar2", static_cast<int64_t>(1) << 62, "Bar Title for long long"},
    });
    report.Uuid("fakeitem");
    // clang-format on

    // serialize
    std::stringstream s;
    report.Print(s);

    // deserialize
    JSON::Json doc;
    s >> doc;
    ASSERT_TRUE(doc.is_object());
    const JSON::Json attributes = doc["attributes"];
    ASSERT_TRUE(attributes.is_array());
    for (const auto& attribute : attributes) {
        ASSERT_TRUE(attribute.is_object());
        const std::string id = attribute["id"];
        if (id == "foo") {
            ASSERT_EQ(attribute["value"], 4294967295);
        } else if (id == "bar") {
            ASSERT_EQ(attribute["value"], 281474976710655);
        } else if (id == "bar2") {
            ASSERT_EQ(attribute["value"], 4611686018427387904);
        } else {
            ASSERT_EQ("unexpected_id", id);
        }
    }
}
