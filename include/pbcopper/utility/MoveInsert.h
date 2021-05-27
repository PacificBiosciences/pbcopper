#ifndef PBCOPPER_UTILITY_MOVEINSERT_H
#define PBCOPPER_UTILITY_MOVEINSERT_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <iterator>

namespace PacBio {
namespace Utility {

/// \brief Inserts contents of 'src' container into 'dst' associative container
///        using move semantics.
///
/// \param[in]      src  Input container that will be empty after execution
/// \param[in,out]  dst  Output container that will be appended to
///
template <typename T, typename U>
void MoveInsert(T&& src, U& dst) noexcept
{
    std::move(src.begin(), src.end(), std::insert_iterator<U>(dst, dst.end()));
    src.clear();
}

/// \brief Inserts contents of 'src' container into 'dst' associative container
///        using move semantics.
///
/// \param[in]      src  Input container that will be empty after execution
/// \param[in,out]  dst  Output container that will be appended to
///
template <typename T>
void MoveInsert(T&& src, T& dst) noexcept
{
    // can short-circuit 'move' into empty dst when types are same
    if (dst.empty()) {
        dst = std::move(src);
    } else {
        std::move(src.begin(), src.end(), std::insert_iterator<T>(dst, dst.end()));
        src.clear();
    }
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_MOVEINSERT_H
