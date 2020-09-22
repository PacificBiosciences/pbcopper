// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_MINMAX_H
#define PBCOPPER_UTILITY_MINMAX_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>

namespace PacBio {
namespace Utility {

//
// Single helper API for finding multi-value maximum or minimum values, rather
// than handrolling macros/methods for each arity.
//
// Examples:
//    Utility::Max(a,b);
//    Utility::Max(a,b,c);
//    Utility::Max(a,b,c,d);
//
//    Utility::Min(a,b);
//    Utility::Min(a,b,c);
//    Utility::Min(a,b,c,d);
//

///
/// \brief Find maximum of 2 values. (End condition of the multi-value Max(...))
///
/// \note Both values must be of the same type.
///
template <typename T>
constexpr T Max(const T& lhs, const T& rhs)
{
    return std::max(lhs, rhs);
}

///
/// \brief Find maximum among a variable number of inputs.
///
/// \note All values must be of the same type.
///
template <typename T, typename... Args>
constexpr T Max(const T& first, const Args&... args)
{
    return Max(first, Max(args...));
}

///
/// \brief Find minimum of 2 values. (End condition of the multi-value Min(...))
///
/// \note Both values must be of the same type.
///
template <typename T>
constexpr T Min(const T& lhs, const T& rhs)
{
    return std::min(lhs, rhs);
}

///
/// \brief Find minimum among a variable number of inputs.
///
/// \note All values must be of the same type.
///
template <typename T, typename... Args>
constexpr T Min(const T& first, const Args&... args)
{
    return Min(first, Min(args...));
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_MINMAX_H
