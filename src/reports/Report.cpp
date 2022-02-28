#include <pbcopper/reports/Report.h>

#include <pbcopper/json/JSON.h>
#include <pbcopper/utility/PbcopperVersion.h>
#include "ReportPrinter.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <array>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <utility>

using Json = PacBio::JSON::Json;

namespace PacBio {
namespace Reports {

// -----------
// Attribute
// -----------

Attribute::Attribute(std::string id, ReportValue value)
    : BaseReportElement{std::move(id)}, value_{std::move(value)}
{}

Attribute::Attribute(std::string id, ReportValue value, std::string name)
    : BaseReportElement{std::move(id)}, name_{std::move(name)}, value_{std::move(value)}
{}

Attribute::operator int() const { return boost::get<int>(value_); }

Attribute::operator unsigned int() const { return boost::get<unsigned int>(value_); }

Attribute::operator double() const { return boost::get<double>(value_); }

Attribute::operator bool() const { return boost::get<bool>(value_); }

Attribute::operator std::string() const { return boost::get<std::string>(value_); }

const std::string& Attribute::Name() const { return name_; }

Attribute& Attribute::Name(std::string name)
{
    name_ = std::move(name);
    return *this;
}

const ReportValue& Attribute::Value() const { return value_; }

Attribute& Attribute::Value(ReportValue value)
{
    value_ = std::move(value);
    return *this;
}

// -------------------
// BaseReportElement
// -------------------

BaseReportElement::BaseReportElement(std::string id) : id_{std::move(id)} {}

const std::string& BaseReportElement::Id() const { return id_; }

// --------
// Column
// --------

Column::Column(std::string id, std::vector<ReportValue> values, std::string header)
    : BaseReportElement{id}, header_{std::move(header)}, values_(std::move(values))
{}

Column& Column::AddValue(ReportValue val)
{
    values_.emplace_back(std::move(val));
    return *this;
}

const std::string& Column::Header() const { return header_; }

Column& Column::Header(std::string header)
{
    header_ = std::move(header);
    return *this;
}

const std::vector<ReportValue>& Column::Values() const { return values_; }

Column& Column::Values(std::vector<ReportValue> values)
{
    values_ = std::move(values);
    return *this;
}

// --------
// Report
// --------

Report::Report(std::string id) : Report{std::move(id), ""} {}

Report::Report(std::string id, std::string title)
    : BaseReportElement{std::move(id)}, title_{std::move(title)}
{
    static boost::uuids::random_generator gen;
    const boost::uuids::uuid uuid = gen();
    uuid_ = boost::uuids::to_string(uuid);
}

Report& Report::AddAttribute(Attribute attribute)
{
    attributes_.emplace_back(std::move(attribute));
    return *this;
}

Report& Report::AddDatasetUuid(std::string uuid)
{
    datasetUuids_.emplace_back(std::move(uuid));
    return *this;
}

Report& Report::AddTable(Table table)
{
    tables_.emplace_back(std::move(table));
    return *this;
}

const std::vector<Attribute>& Report::Attributes() const { return attributes_; }

Report& Report::Attributes(std::vector<Attribute> attributes)
{
    attributes_ = std::move(attributes);
    return *this;
}

const std::vector<std::string>& Report::DatasetUuids() const { return datasetUuids_; }

Report& Report::DatasetUuids(std::vector<std::string> uuids)
{
    datasetUuids_ = std::move(uuids);
    return *this;
}

const std::vector<Table>& Report::Tables() const { return tables_; }

Report& Report::Tables(std::vector<Table> tables)
{
    tables_ = std::move(tables);
    return *this;
}

void Report::Print(const std::string& fn, const std::string& prefix) const
{
    std::ofstream f{fn};
    Print(f, prefix);
}

void Report::Print(std::ostream& out, const std::string& prefix) const
{
    internal::ReportPrinter::Print(out, *this, prefix);
}

void Report::PrintSummary(const std::string& fn) const
{
    std::ofstream f{fn};
    PrintSummary(f);
}

void Report::PrintSummary(std::ostream& out) const
{
    internal::ReportPrinter::PrintSummary(out, *this);
}

const std::string& Report::Title() const { return title_; }

Report& Report::Title(std::string title)
{
    title_ = std::move(title);
    return *this;
}

const std::string& Report::Uuid() const { return uuid_; }

Report& Report::Uuid(std::string uuid)
{
    uuid_ = std::move(uuid);
    return *this;
}

// -------
// Table
// -------

Table::Table(std::string id, std::vector<Column> columns, std::string title)
    : BaseReportElement(std::move(id)), title_(std::move(title)), columns_(std::move(columns))
{}

Table::Table(std::string id) : BaseReportElement(std::move(id)) {}

Table& Table::AddColumn(Column column)
{
    columns_.emplace_back(std::move(column));
    return *this;
}

const std::vector<Column>& Table::Columns() const { return columns_; }

Table& Table::Columns(std::vector<Column> columns)
{
    columns_ = std::move(columns);
    return *this;
}

const std::string& Table::Title() const { return title_; }

Table& Table::Title(std::string title)
{
    title_ = title;
    return *this;
}

}  // namespace Reports
}  // namespace PacBio
