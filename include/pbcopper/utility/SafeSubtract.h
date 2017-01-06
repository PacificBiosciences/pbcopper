#ifndef PBCOPPER_UTILITY_SAFESUBTRACT_H
#define PBCOPPER_UTILITY_SAFESUBTRACT_H

#include <cstddef>

namespace PacBio {
namespace Utility {

///
/// \brief SafeSubtract
/// \param size
/// \param k
/// \return
///
inline size_t SafeSubtract(size_t size, size_t k)
{ return size > k ? size - k : 0; }

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_SAFESUBTRACT_H
