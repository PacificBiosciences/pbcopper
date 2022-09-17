#include "ReportPrinter.h"

#include <pbcopper/json/JSON.h>
#include <pbcopper/reports/Report.h>
#include <pbcopper/utility/PbcopperVersion.h>

#include <ostream>
#include <string_view>

using Json = PacBio::JSON::Json;

namespace {

// clang-format off
Json VariantToJson(const PacBio::Reports::ReportValue& value)
{
    // boost::blank, int, double, bool, std::string
    switch(value.which()) {
        case 0 : return Json{};
        case 1 : return Json::number_integer_t{boost::get<int>(value)};
        case 2 : return Json::number_unsigned_t{boost::get<unsigned int>(value)};
        case 3 : return Json::number_integer_t{boost::get<int64_t>(value)};
        case 4 : return Json::number_float_t{boost::get<double>(value)};
        case 5 : return Json::boolean_t{boost::get<bool>(value)};
        case 6 : return Json::string_t{boost::get<std::string>(value)};
        default:
            throw std::runtime_error{"[pbcopper] reports ERROR: invalid report value type"};
    }
}
// clang-format on

Json ToJson(const PacBio::Reports::Attribute& attribute, const std::string& prefix)
{
    Json result = Json::object();
    result["id"] = prefix + attribute.Id();
    result["name"] = attribute.Name();
    result["value"] = VariantToJson(attribute.Value());
    return result;
}

Json ToJson(const PacBio::Reports::Column& column, const std::string& prefix)
{
    Json result = Json::object();

    result["id"] = prefix + column.Id();
    result["header"] = column.Header();

    Json values = Json::array();
    for (const auto& value : column.Values()) {
        values.emplace_back(VariantToJson(value));
    }
    result["values"] = values;

    return result;
}

Json ToJson(const PacBio::Reports::Table& table, const std::string& prefix)
{
    Json result = Json::object();

    result["id"] = prefix + table.Id();
    result["title"] = table.Title();

    Json columns = Json::array();
    for (const auto& col : table.Columns()) {
        columns.emplace_back(ToJson(col, prefix));
    }
    result["columns"] = columns;

    return result;
}

}  // namespace

namespace PacBio {
namespace Reports {
namespace internal {

constexpr std::string_view PB_REPORT_SCHEMA_VERSION{"1.0.1"};

void ReportPrinter::Print(std::ostream& out, const Report& report, const std::string& prefix)
{
    Json result = Json::object();

    result["_comment"] = "Created by pbcopper v" + Utility::LibraryVersionString();
    result["version"] = PB_REPORT_SCHEMA_VERSION;
    result["id"] = prefix + report.Id();
    result["title"] = report.Title();
    result["uuid"] = report.Uuid();
    result["dataset_uuids"] = report.DatasetUuids();

    Json attributes = Json::array();
    for (const auto& attr : report.Attributes()) {
        attributes.emplace_back(ToJson(attr, prefix));
    }
    result["attributes"] = attributes;

    Json tables = Json::array();
    for (const auto& table : report.Tables()) {
        tables.emplace_back(ToJson(table, prefix));
    }
    result["tables"] = tables;

    result["plotGroups"] = Json::array();

    out << result.dump(4);
}

void ReportPrinter::PrintSummary(std::ostream& out, const Report& report)
{
    Json result = Json::object();
    for (const auto& attr : report.Attributes()) {
        result[attr.Id()] = VariantToJson(attr.Value());
    }
    out << result.dump(4);
}

}  // namespace internal
}  // namespace Reports
}  // namespace PacBio
