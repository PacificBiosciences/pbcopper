#ifndef PBCOPPER_CLI_v2_HELPMETRICS_H
#define PBCOPPER_CLI_v2_HELPMETRICS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/HiddenOptionMode.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>

#include <string>
#include <unordered_map>

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

    HelpMetrics(const Interface& interface, HiddenOptionMode hiddenOptionMode);
    HelpMetrics(const MultiToolInterface& interface, HiddenOptionMode hiddenOptionMode);

    HelpMetrics(const Interface& interface, size_t explicitMaxColumn,
                HiddenOptionMode hiddenOptionMode);
    HelpMetrics(const MultiToolInterface& interface, size_t explicitMaxColumn,
                HiddenOptionMode hiddenOptionMode);

    void Calculate(const Interface& interface);
    void Calculate(const MultiToolInterface& interface);
    std::string HelpEntry(const OptionData& option) const;
    std::string HelpEntry(std::string name, std::string type, const std::string& description) const;
    void UpdateForOption(const OptionData& option);

    std::unordered_map<OptionData, FormattedEntry> FormattedOptionNames;
    std::unordered_map<PositionalArgumentData, FormattedEntry> FormattedPosArgNames;
    size_t MaxNameLength = 0;
    size_t MaxTypeLength = 0;
    size_t MaxColumn = 79;
    bool ShowHiddenOptions = false;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_HELPMETRICS_H
