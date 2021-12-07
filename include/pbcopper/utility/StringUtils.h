#ifndef PBCOPPER_UTILITY_STRINGUTILS_H
#define PBCOPPER_UTILITY_STRINGUTILS_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

#include <cstdint>

namespace PacBio {
namespace Utility {

///
/// Concatenates input strings into a single string, separated by a string
/// separator
///
std::string Join(const std::vector<std::string>& input, const std::string& separator);

///
/// Concatenates input strings into a single string, separated by a char
/// separator
///
std::string Join(const std::vector<std::string>& input, const char separator);

///
/// Converts a string with (optional) scientific notation (e.g. 1e6)
/// to its integer representation.
///
/// \throws if input is empty or invalid format
///
int64_t ScientificNotationToInt(const std::string& input);

///
/// Converts a string with (optional) scientific notation (e.g. 1e6)
/// to its unsigned integer representation.
///
/// \throws if input is empty or invalid format
///
uint64_t ScientificNotationToUInt(const std::string& input);

///
/// Converts a string with (optional) scientific notation (e.g. 1e6)
/// to its double representation.
///
/// \throws if input is empty or invalid format
///
double ScientificNotationToDouble(const std::string& input);

///
/// Converts a string with (optional) SI multiplier suffixes (e.g. "20k", "100M")
/// to its integer representation.
///
/// \throws if input is empty or invalid format
///
int64_t SIStringToInt(const std::string& input);

///
/// Converts a string with (optional) SI multiplier suffixes (e.g. "20k", "100M")
/// to its unsigned integer representation.
///
/// \throws if input is empty or invalid format
///
uint64_t SIStringToUInt(const std::string& input);

///
/// Converts a string with (optional) SI multiplier suffixes (e.g. "2.5k", "100M")
/// to its double representation.
///
/// \throws if input is empty or invalid format
///
double SIStringToDouble(const std::string& input);

///
/// Returns the list of 'words' from a string, separated by a char delimiter
///
std::vector<std::string> Split(const std::string& line, const char delim = '\t');

///
/// Generates a string from the input, with newline chars inserted such that no
/// line exceeds the requested width. If a particular word covers this boundary,
/// the newline is inserted before that word.
///
std::string WordWrapped(const std::string& input, const size_t maxColumns);

///
/// Generates a list of strings from the input, such that no string element
/// exceeds the requested width. If a particular word covers this boundary,
/// then a new string is created, beginning with that word.
///
std::vector<std::string> WordWrappedLines(const std::string& input, const size_t maxColumns);

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_STRINGUTILS_H
