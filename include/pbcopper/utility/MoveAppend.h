// Author: Armin Töpfer

#ifndef PBCOPPER_UTILITY_MOVEAPPEND_H
#define PBCOPPER_UTILITY_MOVEAPPEND_H

#include <pbcopper/PbcopperConfig.h>

#include <iterator>
#include <vector>

namespace PacBio {
namespace Utility {

/// \brief Appends content of src vector to dst vector using move semantics.
///
/// \param[in]      src  Input vector that will be empty after execution
/// \param[in,out]  dst  Output vector that will be appended to
///
template <typename T>
inline void MoveAppend(std::vector<T>& src, std::vector<T>& dst) noexcept
{
    if (dst.empty()) {
        dst = std::move(src);
    } else {
        dst.reserve(dst.size() + src.size());
        std::move(src.begin(), src.end(), std::back_inserter(dst));
        src.clear();
    }
}

/// \brief Appends content of src vector to dst vector using move semantics.
///
/// \param[in]      src  Input vector via perfect forwarding
/// \param[in,out]  dst  Output vector that will be appended to
///
template <typename T>
inline void MoveAppend(std::vector<T>&& src, std::vector<T>& dst) noexcept
{
    if (dst.empty()) {
        dst = std::move(src);
    } else {
        dst.reserve(dst.size() + src.size());
        std::move(src.begin(), src.end(), std::back_inserter(dst));
        src.clear();
    }
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_MOVEAPPEND_H
