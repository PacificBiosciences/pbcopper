#ifndef PBCOPPER_UTILITY_STRINGUTILS_INL_H
#define PBCOPPER_UTILITY_STRINGUTILS_INL_H

#include "pbcopper/utility/StringUtils.h"
#include <sstream>

namespace PacBio {
namespace Utility {

inline std::vector<std::string> Split(const std::string& line,
                                      const char delim)
{
    auto tokens     = std::vector<std::string>{ };
    auto lineStream = std::stringstream{ line };
    auto token      = std::string{ };
    while (std::getline(lineStream, token, delim))
        tokens.push_back(token);
    return tokens;
}

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_STRINGUTILS_INL_H
