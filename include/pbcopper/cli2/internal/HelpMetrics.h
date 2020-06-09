// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_HELPMETRICS_H
#define PBCOPPER_CLI_v2_HELPMETRICS_H

#include <string>
#include <unordered_map>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Terminal & formatted text widths
///
struct FormattedEntry
{
    std::string nameString;  // e.g. "-h,--help"
    std::string typeString;  // e.g. "STR"
};

class HelpMetrics
{
public:
    static size_t TestingFixedWidth;

    static std::string OptionNames(const OptionData& option);

    explicit HelpMetrics(const Interface& interface);
    explicit HelpMetrics(const MultiToolInterface& interface);
    HelpMetrics(const Interface& interface, const size_t explicitMaxColumn);
    HelpMetrics(const MultiToolInterface& interface, const size_t explicitMaxColumn);

    void Calculate(const Interface& interface);
    void Calculate(const MultiToolInterface& interface);
    std::string HelpEntry(const OptionData& option) const;
    std::string HelpEntry(std::string name, std::string type, const std::string& description) const;
    void UpdateForOption(const OptionData& option);

    std::unordered_map<OptionData, FormattedEntry> formattedOptionNames;
    std::unordered_map<PositionalArgumentData, FormattedEntry> formattedPosArgNames;
    size_t maxNameLength = 0;
    size_t maxTypeLength = 0;
    size_t maxColumn = 79;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_HELPMETRICS_H
