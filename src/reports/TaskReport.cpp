#include <pbcopper/reports/Report.h>

#include <exception>
#include <fstream>
#include <string>

#if defined(_POSIX_VERSION)
#include <unistd.h>
#endif

namespace PacBio {
namespace Reports {

void writeTaskReport(std::string jsonFile, int nproc, double runTime, int exitCode)
{
#if defined(_POSIX_VERSION)
    char hostname_[255];
    int rc = gethostname(hostname_, 255);
    if (rc != 0) throw std::runtime_error("gethostname() failed");
    std::string hostname(hostname_);
#else
    std::string hostname = "unknown";
#endif
    Report report{"workflow_task", "Task Report"};
    report.Attributes({{"host", hostname, "Hostname"},
                       {"nproc", nproc, "Number of cores/slots"},
                       {"run_time", runTime, "Run time (seconds)"},
                       {"exit_code", exitCode, "Exit code"}});
    std::ofstream jsonOut(jsonFile);
    report.Print(jsonOut);
}
}
}
