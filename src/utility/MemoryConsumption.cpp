// Authors: Armin Töpfer, Derek Barnett

#include <pbcopper/utility/MemoryConsumption.h>

#include <sys/resource.h>

namespace PacBio {
namespace Utility {

int64_t MemoryConsumption::PeakRss()
{
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);
#ifdef __linux__
    return r.ru_maxrss * 1024;
#else
    return r.ru_maxrss;
#endif
}

}  // namespace Utility
}  // namespace PacBio
