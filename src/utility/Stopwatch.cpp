#include <pbcopper/utility/Stopwatch.h>

#include <sstream>
#include <type_traits>

namespace PacBio {
namespace Utility {

std::string Stopwatch::PrettyPrintNanoseconds(int64_t nanosecs)
{
    auto d = nanosecs / 1000 / 1000 / 1000 / 60 / 60 / 24;
    auto h = (nanosecs / 1000 / 1000 / 1000 / 60 / 60) % 24;
    auto m = (nanosecs / 1000 / 1000 / 1000 / 60) % 60;
    auto s = (nanosecs / 1000 / 1000 / 1000) % 60;
    auto ms = (nanosecs / 1000 / 1000) % 1000;
    auto us = (nanosecs / 1000) % 1000;
    auto ns = nanosecs % 1000;
    std::ostringstream ss;
    if (d > 0) {
        ss << std::to_string(d) << "d ";
    }
    if (h > 0) {
        ss << std::to_string(h) << "h ";
    }
    if (m > 0 && d == 0) {
        ss << std::to_string(m) << "m ";
    }
    if (s > 0 && h == 0) {
        ss << std::to_string(s) << "s ";
    }
    if (ms > 0 && m == 0) {
        ss << std::to_string(ms) << "ms ";
    }
    if (us > 0 && s == 0) {
        ss << std::to_string(us) << "us ";
    }
    if (ns > 0 && ms == 0) {
        ss << std::to_string(ns) << "ns ";
    }
    return ss.str();
}

double Stopwatch::CpuTime()
{
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);
    return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}
}  // namespace Utility
}  // namespace PacBio
