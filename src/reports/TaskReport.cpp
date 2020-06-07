#include <pbcopper/reports/Report.h>

#include <exception>
#include <ostream>
#include <string>

#if defined(_POSIX_VERSION)
#include <unistd.h>
#endif

namespace PacBio {
namespace Reports {

TaskReport::TaskReport(int nproc, double runTime, int exitCode)
    : report_{"workflow_task", "Task Report"}
{
#if defined(_POSIX_VERSION)
    char hostname_[255];
    int rc = gethostname(hostname_, 255);
    if (rc != 0) throw std::runtime_error{"[pbcopper] reports ERROR: gethostname() failed"};
    std::string hostname(hostname_);
#else
    std::string hostname = "unknown";
#endif
    report_.Attributes({{"host", hostname, "Hostname"},
                        {"nproc", nproc, "Number of cores/slots"},
                        {"run_time", runTime, "Run time (seconds)"},
                        {"exit_code", exitCode, "Exit code"}});
}

void TaskReport::Print(const std::string& fn, const std::string& prefix) const
{
    report_.Print(fn, prefix);
}

void TaskReport::Print(std::ostream& out, const std::string& prefix) const
{
    report_.Print(out, prefix);
}

}  // namespace Reports
}  // namespace PacBio
