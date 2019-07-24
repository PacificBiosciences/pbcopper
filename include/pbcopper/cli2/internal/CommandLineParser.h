// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_COMMANDLINEPARSER_H
#define PBCOPPER_CLI_v2_COMMANDLINEPARSER_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <unordered_map>
#include <vector>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/OptionData.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Parses option & positional argument values from the command line.
///
class CommandLineParser
{
public:
    ///
    /// Initialize with application interface
    ///
    explicit CommandLineParser(Interface interface);

    ///
    /// Parse this command line input for results
    ///
    Results Parse(const std::vector<std::string>& arguments);

private:
    ///
    /// Determine an option's value given a long option (e.g. "--reference")
    ///
    void ParseLongOption(const std::string& arg, std::deque<std::string>& args, Results& results);

    ///
    /// Determine an option's value given a short option (e.g. "-r")
    ///
    void ParseShortOption(const std::string& arg, std::deque<std::string>& args, Results& results);

    ///
    /// Look up option data for a command-line token. Throws if not found.
    ///
    const OptionData& OptionFor(const std::string& name) const;

private:
    Interface interface_;
    std::vector<OptionData> options_;
    std::unordered_map<std::string, OptionData> optionsByName_;  // name -> OptionData
    size_t currentPosArgNumber_ = 0;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_COMMANDLINEPARSER_H
