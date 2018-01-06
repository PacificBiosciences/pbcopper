#ifndef PBCOPPER_UTILITY_UNUSED_H
#define PBCOPPER_UTILITY_UNUSED_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

template<typename T> void UNUSED(const T&) { }

} // namespace Utility
} // namespace PacBio

// Convenience using-declaration
//
// So you can use simple use UNUSED(x) rather than
//
// PacBio::Utility::UNUSED(x)
//
// or
//
// using PacBio::Utility::UNUSED;
// UNUSED(x);
//
// every where it is used.
//
using PacBio::Utility::UNUSED;

#endif // PBCOPPER_UTILITY_UNUSED_H
