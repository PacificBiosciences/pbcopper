#ifndef PBCOPPER_DATA_POSITION_H
#define PBCOPPER_DATA_POSITION_H

#include <pbcopper/PbcopperConfig.h>

#include <limits>

#include <cstdint>

namespace PacBio {
namespace Data {

typedef int32_t Position;

static const Position UnmappedPosition = Position{-1};

constexpr Position MaximumPosition = std::numeric_limits<decltype(MaximumPosition)>::max();

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_POSITION_H
