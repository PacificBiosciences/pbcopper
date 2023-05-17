#ifndef PBCOPPER_UTILITY_MEMORYCONSUMPTION_H
#define PBCOPPER_UTILITY_MEMORYCONSUMPTION_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

namespace PacBio {
namespace Utility {

struct MemoryConsumption
{
    static std::int64_t PeakRss();
};

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_MEMORYCONSUMPTION_H
