#ifndef PBCOPPER_UTILITY_STRINGUTILS_INL_H
#define PBCOPPER_UTILITY_STRINGUTILS_INL_H

#include <sstream>

#include "pbcopper/utility/StringUtils.h"

namespace PacBio {
namespace Utility {

inline std::string Join(const std::vector<std::string>& input,
                        const std::string& delim)
{
    // determine total joined length
    size_t totalLen = 0;
    for (const auto& s : input)
        totalLen += s.size();
    if (!input.empty())
        totalLen += delim.size() * (input.size()-1); // no delim after last string

    // join input strings
    std::string result;
    result.reserve(totalLen);
    for (size_t i = 0; i < input.size(); ++i) {
        if (i != 0)
            result += delim;
        result += input.at(i);
    }
    return result;
}

inline std::string Join(const std::vector<std::string>& input,
                        const char delim)
{
    return Join(input, std::string(1, delim));
}

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
