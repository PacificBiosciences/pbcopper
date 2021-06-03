#ifndef PBCOPPER_UTILITY_SSIZE_H
#define PBCOPPER_UTILITY_SSIZE_H

#include <pbcopper/PbcopperConfig.h>

#include <iterator>
#include <tuple>

namespace PacBio {
namespace Utility {

//
// Spoofing std::ssize(). Will pass through to STL, if C++20 and beyond
//
// Using implementation examples from:
//     https://en.cppreference.com/w/cpp/iterator/size
//

// C++ 20 and beyond
#if __cplusplus >= 202002L

template <class C>
constexpr auto Ssize(const C& c)
    -> std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>
{
    return std::ssize(c);
}

template <class T, std::ptrdiff_t N>
constexpr std::ptrdiff_t Ssize(const T (&array)[N]) noexcept
{
    return std::ssize(array);
}

// C++11/14/17
#else

template <class C>
constexpr auto Ssize(const C& c)
    -> std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>
{
    using R = std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>;
    return static_cast<R>(c.size());
}

template <class T, std::ptrdiff_t N>
constexpr std::ptrdiff_t Ssize(const T (&array)[N]) noexcept
{
    std::ignore = array;
    return N;
}

#endif

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_SSIZE_H
