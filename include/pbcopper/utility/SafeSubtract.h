#ifndef PBCOPPER_UTILITY_SAFESUBTRACT_H
#define PBCOPPER_UTILITY_SAFESUBTRACT_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>

namespace PacBio {
namespace Utility {

///
/// \brief SafeSubtract
/// \param size
/// \param k
/// \return
///
inline std::size_t SafeSubtract(std::size_t size, std::size_t k) { return size > k ? size - k : 0; }

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_SAFESUBTRACT_H
