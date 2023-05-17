#ifndef PBCOPPER_DATA_POSITION_H
#define PBCOPPER_DATA_POSITION_H

#include <pbcopper/PbcopperConfig.h>

#include <limits>

#include <cstdint>

namespace PacBio {
namespace Data {

using Position = std::int32_t;

constexpr inline Position UNMAPPED_POSITION = -1;
constexpr inline Position MAXIMUM_POSITION = std::numeric_limits<decltype(MAXIMUM_POSITION)>::max();

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_POSITION_H
