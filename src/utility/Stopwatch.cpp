// Author: Derek Barnett
#include <pbcopper/utility/Stopwatch.h>

#include <cassert>
#include <sstream>
#include <type_traits>

namespace PacBio {
namespace Utility {

static_assert(std::is_copy_constructible<Stopwatch>::value,
              "Stopwatch(const Stopwatch&) is not = default");
static_assert(std::is_copy_assignable<Stopwatch>::value,
              "Stopwatch& operator=(const Stopwatch&) is not = default");

static_assert(std::is_nothrow_move_constructible<Stopwatch>::value,
              "Stopwatch(Stopwatch&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Stopwatch>::value,
              "Stopwatch& operator=(Stopwatch&&) is not = noexcept");

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
    if (d > 0) ss << std::to_string(d) << "d ";
    if (h > 0) ss << std::to_string(h) << "h ";
    if (m > 0 && d == 0) ss << std::to_string(m) << "m ";
    if (s > 0 && h == 0) ss << std::to_string(s) << "s ";
    if (ms > 0 && m == 0) ss << std::to_string(ms) << "ms ";
    if (us > 0 && s == 0) ss << std::to_string(us) << "us ";
    if (ns > 0 && ms == 0) ss << std::to_string(ns) << "ns ";
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
