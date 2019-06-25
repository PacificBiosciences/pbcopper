// Author: Nat Echols

#include <gtest/gtest.h>
#include <pbcopper/reports/Report.h>

#include <sstream>
#include <string>
#include <vector>

#include <pbcopper/utility/PbcopperVersion.h>

// clang-format off

using namespace PacBio;
using namespace PacBio::Reports;

namespace ReportsTests {

Report MakeReport()
{
    const int foo_value = 3;
    const std::string bar_value = "test";
    const double baz_value = 0.75;

    Report report{"isoseq3_flnca", "Report isoseq3"};
    report.Attributes({
        {"foo", foo_value, "Foo Title"},
        {"bar", bar_value, "Bar Title"},
        {"baz", baz_value, "Baz Title"}
    });
    report.Uuid("testable_uuid");

    Table table{"table1"};
    table.AddColumn(Column{"col1", std::vector<ReportValue>{1, 2, 3, 4}});
    report.AddTable(table);

    return report;
}

}  // namespace ReportsTests

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

    std::stringstream s;
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


TEST(Reports_Report, can_write_task_report)
{
    writeTaskReport("task-report.json", 16, 100, 0);
}
// clang-format on
