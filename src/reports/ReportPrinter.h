// Author: Derek Barnett

// see pbcommand/schemas/pbreport.avsc

#ifndef PBCOPPER_REPORTS_REPORTPRINTER_H
#define PBCOPPER_REPORTS_REPORTPRINTER_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
#include <string>

namespace PacBio {
namespace Reports {

class Report;

namespace internal {

class ReportPrinter
{
public:
    static void Print(std::ostream& out, const Report& report, const std::string& prefix);
    static void PrintSummary(std::ostream& out, const Report& report);
};

}  // namespace internal
}  // namespace Reports
}  // namespace PacBio

#endif  // PBCOPPER_REPORTS_REPORTPRINTER_H
