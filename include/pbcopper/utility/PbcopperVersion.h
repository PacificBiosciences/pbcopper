// Author: Armin Töpfer

#ifndef PBCOPPER_UTILITY_PBCOPPERVERSION_H
#define PBCOPPER_UTILITY_PBCOPPERVERSION_H

#include <string>
#include <tuple>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

std::string LibraryGitSha1String();
std::string LibraryVersionString();
std::tuple<int, int, int> LibraryVersionTriple();
}
}  // ::PacBio::Utility

#endif  // PBCOPPER_UTILITY_PBCOPPERVERSION_H
