#ifndef PBCOPPER_REPORTS_REPORT_H
#define PBCOPPER_REPORTS_REPORT_H

// see pbcommand/schemas/pbreport.avsc

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
#include <string>
#include <variant>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace Reports {

using ReportValue =
    std::variant<std::monostate, int, unsigned int, int64_t, double, bool, std::string>;

class BaseReportElement
{
public:
    explicit BaseReportElement(std::string id);
    const std::string& Id() const;

protected:
    std::string id_;
};

class Attribute final : public BaseReportElement
{
public:
    Attribute(std::string id, ReportValue value);
    Attribute(std::string id, ReportValue value, std::string name);

    // conversion
    operator int() const;
    operator unsigned int() const;
    operator int64_t() const;
    operator double() const;
    operator bool() const;
    operator std::string() const;

    // name
    const std::string& Name() const;
    Attribute& Name(std::string name);

    // value
    const ReportValue& Value() const;
    Attribute& Value(ReportValue value);

private:
    std::string name_;
    ReportValue value_;
};

class Column : public BaseReportElement
{
public:
    Column(std::string id, std::vector<ReportValue> values, std::string header = "");

    // header
    const std::string& Header() const;
    Column& Header(std::string header);

    // values
    Column& AddValue(ReportValue value);
    const std::vector<ReportValue>& Values() const;
    Column& Values(std::vector<ReportValue> values);

private:
    std::string header_;
    std::vector<ReportValue> values_;
};

class Table : public BaseReportElement
{
public:
    explicit Table(std::string id);
    Table(std::string id, std::vector<Column> columns, std::string title = "");

    // title
    const std::string& Title() const;
    Table& Title(std::string title);

    // columns
    Table& AddColumn(Column column);
    const std::vector<Column>& Columns() const;
    Table& Columns(std::vector<Column> columns);

private:
    std::string title_;
    std::vector<Column> columns_;
};

class Report : public BaseReportElement
{
public:
    explicit Report(std::string id);
    Report(std::string id, std::string title);

    // title
    const std::string& Title() const;
    Report& Title(std::string title);

    // uid
    const std::string& Uuid() const;
    Report& Uuid(std::string uuid);

    // attributes
    Report& AddAttribute(Attribute attribute);
    const std::vector<Attribute>& Attributes() const;
    Report& Attributes(std::vector<Attribute> attributes);

    // dataset uuids
    Report& AddDatasetUuid(std::string uuid);
    const std::vector<std::string>& DatasetUuids() const;
    Report& DatasetUuids(std::vector<std::string> uuids);

    // table
    Report& AddTable(Table table);
    const std::vector<Table>& Tables() const;
    Report& Tables(std::vector<Table> tables);

    // print pbreport format
    void Print(const std::string& fn, const std::string& prefix = "") const;
    void Print(std::ostream& out, const std::string& prefix = "") const;

    // print (diffable) attributes only
    void PrintSummary(const std::string& fn) const;
    void PrintSummary(std::ostream& out) const;

private:
    std::string title_;
    std::string uuid_;
    std::vector<std::string> datasetUuids_;
    std::vector<Attribute> attributes_;
    std::vector<Table> tables_;
};

// utility functions
class TaskReport
{
public:
    TaskReport(int nproc, double runTime, int exitCode, int64_t peakRss);

    void Print(const std::string& fn, const std::string& prefix = "") const;
    void Print(std::ostream& out, const std::string& prefix = "") const;

private:
    Report report_;
};

}  // namespace Reports
}  // namespace PacBio

#endif  // PBCOPPER_REPORTS_REPORT_H
