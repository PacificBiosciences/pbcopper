#ifndef PBCOPPER_UTILITY_PBCOPPERVERSION_H
#define PBCOPPER_UTILITY_PBCOPPERVERSION_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <tuple>

namespace PacBio {
namespace Utility {

std::string LibraryGitSha1String();
std::string LibraryVersionString();
std::tuple<int, int, int> LibraryVersionTriple();
}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_PBCOPPERVERSION_H
