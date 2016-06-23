#ifndef PBCOPPER_UTILITY_STRINGUTILS_INL_H
#define PBCOPPER_UTILITY_STRINGUTILS_INL_H

#include "pbcopper/utility/StringUtils.h"
#include <sstream>

namespace PacBio {
namespace Utility {

inline std::vector<std::string> Split(const std::string& line,
                                      const char delim)
{
    std::vector<std::string> tokens;
    std::stringstream lineStream{ line };
    std::string token;
    while (std::getline(lineStream, token, delim))
        tokens.push_back(token);
    return tokens;
}

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_STRINGUTILS_INL_H
