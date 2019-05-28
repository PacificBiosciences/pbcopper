// Author: David Seifert, Derek Barnett

// Reduce the number of exposed symbols in order to speed up
// DSO load times
// https://gcc.gnu.org/wiki/Visibility

#ifndef PBCOPPER_PBCOPPERCONFIG_H
#define PBCOPPER_PBCOPPERCONFIG_H

#include <string>
#include <type_traits>

// string& operator= (string&& str)
// has been made noexcept only in LWG 2063:
//   http://cplusplus.github.io/LWG/lwg-defects.html#2063
// GCC 6+'s "C++11"/SSO std::string is noexcept, but the COW std::string used
// by RHEL devtoolset-6 isn't, and as such we have to make this conditional
// See also
//   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58265#c13
#define PBCOPPER_NOEXCEPT_MOVE_ASSIGN noexcept(std::is_nothrow_move_assignable<std::string>::value)

// clang-format off
#if defined _WIN32 || defined __CYGWIN__
#  ifdef PBCOPPER_BUILDING_LIBRARY
#    ifdef __GNUC__
#      define PBCOPPER_PUBLIC_API __attribute__((dllexport))
#    else
#      define PBCOPPER_PUBLIC_API __declspec(dllexport)  // Note: gcc seems to also supports this syntax
#    endif
#  else
#    ifdef __GNUC__
#      define PBCOPPER_PUBLIC_API __attribute__((dllimport))
#    else
#      define PBCOPPER_PUBLIC_API __declspec(dllimport)  // Note: gcc seems to also supports this syntax
#    endif
#  endif
#  define PBCOPPER_PRIVATE_API
#else
#  if __GNUC__ >= 4
#    define PBCOPPER_PUBLIC_API __attribute__((visibility("default")))
#    define PBCOPPER_PRIVATE_API __attribute__((visibility("hidden")))
#  else
#    define PBCOPPER_PUBLIC_API
#    define PBCOPPER_PRIVATE_API
#  endif
#endif
// clang-format on

#endif  // PBCOPPER_PBCOPPERCONFIG_H
