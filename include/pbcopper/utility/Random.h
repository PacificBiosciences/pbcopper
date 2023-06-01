#ifndef PBCOPPER_UTILITY_RANDOM_H
#define PBCOPPER_UTILITY_RANDOM_H
#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <iterator>

#include <cstdint>

namespace PacBio {
namespace Utility {

uint64_t WyHash64Step(std::uint64_t& seed) noexcept;

}  // namespace Utility
}  // namespace PacBio

#endif
