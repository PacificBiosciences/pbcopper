#ifndef PBCOPPER_ALIGN_FILTERHOMOPOLYMERS_H
#define PBCOPPER_ALIGN_FILTERHOMOPOLYMERS_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Align {
namespace Default {

#ifdef FILTERHOMOPOLYMERS
constexpr bool FILTER_HOMOPOLYMERS = true;
#else
constexpr bool FILTER_HOMOPOLYMERS = false;
#endif

}  // namespace Default
}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_FILTERHOMOPOLYMERS_H
