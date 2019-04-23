#include <pbcopper/utility/StringUtils.h>

#include <cstddef>
#include <sstream>

namespace PacBio {
namespace Utility {

std::string Join(const std::vector<std::string>& input, const std::string& delim)
{
    // determine total joined length
    size_t totalLen = 0;
    for (const auto& s : input)
        totalLen += s.size();
    if (!input.empty())
        totalLen += delim.size() * (input.size() - 1);  // no delim after last string

    // join input strings
    std::string result;
    result.reserve(totalLen);
    for (size_t i = 0; i < input.size(); ++i) {
        if (i != 0) result += delim;
        result += input.at(i);
    }
    return result;
}

std::string Join(const std::vector<std::string>& input, const char delim)
{
    return Join(input, std::string(1, delim));
}

std::vector<std::string> Split(const std::string& line, const char delim)
{
    std::vector<std::string> tokens;
    std::istringstream lineStream{line};
    std::string token;
    while (std::getline(lineStream, token, delim))
        tokens.push_back(token);
    return tokens;
}

std::string WordWrapped(const std::string& input, const size_t maxColumns)
{
    std::ostringstream result;

    std::string word;
    std::istringstream in{input};

    // grab initial (maybe non-existing) word
    if (in >> word) {

        result << word;
        size_t available = maxColumns - word.size();

        // continue until input exhausted
        while (in >> word) {

            const size_t wordLength = word.size();

            // line break before word if overhanging
            if (available < (wordLength + 1)) {
                result << '\n' << word;
                available = maxColumns - wordLength;
            }
            // pass through word, with space
            else {
                result << ' ' << word;
                available -= (wordLength + 1);
            }
        }
    }
    return result.str();
}

std::vector<std::string> WordWrappedLines(const std::string& input, const size_t maxColumns)
{
    return PacBio::Utility::Split(WordWrapped(input, maxColumns), '\n');
}

}  // namespace Utility
}  // namespace PacBio