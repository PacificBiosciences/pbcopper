#ifndef PBCOPPER_UTILITY_MOVEAPPEND_H
#define PBCOPPER_UTILITY_MOVEAPPEND_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <iterator>

namespace PacBio {
namespace Utility {

/// \brief Appends contents of 'src' container to 'dst' sequential container
///        using move semantics.
///
/// \note Current implementation does support deque/list sequences.
///
/// \param[in]      src  Input container that will be empty after execution
/// \param[in,out]  dst  Output container that will be appended to
///
template <typename T, typename U>
auto MoveAppend(T&& src, U& dst)
{
    dst.reserve(dst.size() + src.size());
    std::move(src.begin(), src.end(), std::back_inserter(dst));
    src.clear();
}

/// \brief Appends contents of 'src' container to 'dst' sequential container
///        using move semantics.
///
/// \note Current implementation does support deque/list sequences.
///
/// \param[in]      src  Input container that will be empty after execution
/// \param[in,out]  dst  Output container that will be appended to
///
template <typename T>
auto MoveAppend(T&& src, T& dst)
{
    // can short-circuit 'move' into empty dst when types are same
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
