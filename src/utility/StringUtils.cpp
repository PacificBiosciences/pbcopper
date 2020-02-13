#include <pbcopper/utility/StringUtils.h>

#include <cctype>
#include <cstddef>
#include <sstream>
#include <stdexcept>

namespace PacBio {
namespace Utility {

std::string Join(const std::vector<std::string>& input, const std::string& separator)
{
    // determine total joined length
    size_t totalLen = 0;
    for (const auto& s : input)
        totalLen += s.size();
    if (!input.empty())
        totalLen += separator.size() * (input.size() - 1);  // no separator after last string

    // join input strings
    std::string result;
    result.reserve(totalLen);
    for (size_t i = 0; i < input.size(); ++i) {
        if (i != 0) result += separator;
        result += input.at(i);
    }
    return result;
}

std::string Join(const std::vector<std::string>& input, const char separator)
{
    return Join(input, std::string(1, separator));
}

int64_t SIStringToInt(const std::string& input)
{
    if (input.empty()) {
        throw std::runtime_error{
            "[pbcopper] string utility ERROR: cannot convert empty string to number."};
    }

    const auto suffix = input.back();
    if (!std::isalpha(suffix)) return std::stoll(input);

    int64_t result = std::stoll(input.substr(0, input.size() - 1));
    switch (suffix) {
        case 'K':
        case 'k':
            result *= 1000;
            break;
        case 'M':
        case 'm':
            result *= (1000 * 1000);
            break;
        case 'G':
        case 'g':
            result *= (1000 * 1000 * 1000);
            break;
        default:
            std::ostringstream msg;
            msg << "[pbcopper] string utility ERROR: cannot convert string '" << input
                << "' to number, suffix '" << suffix << "' is not a recognized multipler.";
            throw std::runtime_error{msg.str()};
    }
    return result;
}

uint64_t SIStringToUInt(const std::string& input)
{
    if (input.empty()) {
        throw std::runtime_error{
            "[pbcopper] string utility ERROR: cannot convert empty string to number."};
    }

    const auto suffix = input.back();
    if (!std::isalpha(suffix)) return std::stoull(input);

    uint64_t result = std::stoull(input.substr(0, input.size() - 1));
    switch (suffix) {
        case 'K':
        case 'k':
            result *= 1000;
            break;
        case 'M':
        case 'm':
            result *= (1000 * 1000);
            break;
        case 'G':
        case 'g':
            result *= (1000 * 1000 * 1000);
            break;
        default:
            std::ostringstream msg;
            msg << "[pbcopper] string utility ERROR: cannot convert string '" << input
                << "' to number, suffix '" << suffix << "' is not a recognized multipler.";
            throw std::runtime_error{msg.str()};
    }
    return result;
}

double SIStringToDouble(const std::string& input)
{
    if (input.empty()) {
        throw std::runtime_error{
            "[pbcopper] string utility ERROR: cannot convert empty string to number."};
    }

    const auto suffix = input.back();
    if (!std::isalpha(suffix)) return std::stod(input);

    double result = std::stod(input.substr(0, input.size() - 1));
    switch (suffix) {
        case 'K':
        case 'k':
            result *= 1000.;
            break;
        case 'M':
        case 'm':
            result *= (1000. * 1000.);
            break;
        case 'G':
        case 'g':
            result *= (1000. * 1000. * 1000.);
            break;
        default:
            std::ostringstream msg;
            msg << "[pbcopper] string utility ERROR: cannot convert string '" << input
                << "' to number, suffix '" << suffix << "' is not a recognized multipler.";
            throw std::runtime_error{msg.str()};
    }
    return result;
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
