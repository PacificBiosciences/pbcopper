// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_STRINGUTILS_H
#define PBCOPPER_UTILITY_STRINGUTILS_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

namespace PacBio {
namespace Utility {

std::string Join(const std::vector<std::string>& input, const std::string& delim);

std::string Join(const std::vector<std::string>& input, const char delim);

std::vector<std::string> Split(const std::string& line, const char delim = '\t');

std::string WordWrapped(const std::string& input, const size_t maxColumns);

std::vector<std::string> WordWrappedLines(const std::string& input, const size_t maxColumns);

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_STRINGUTILS_H
