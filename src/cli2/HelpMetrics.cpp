#include <pbcopper/cli2/internal/HelpMetrics.h>

#include <pbcopper/cli2/OptionValue.h>
#include <pbcopper/utility/StringUtils.h>

#include <algorithm>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

#include <cassert>

#include <sys/ioctl.h>
#include <unistd.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

namespace {

std::size_t AdjustedMaxColumn(std::size_t maxColumn)
{
    // if column count is explicitly set (i.e. for testing), use that
    if (HelpMetrics::TestingFixedWidth != 0) {
        return HelpMetrics::TestingFixedWidth;
    }

    // otherwise determine column count from terminal width (default behavior)
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    if (ws.ws_col >= 2) {
        maxColumn = ws.ws_col - 1;
    }
    constexpr std::size_t MAX_COLUMN = 119;
    return std::min(maxColumn, MAX_COLUMN);
}

std::string DisplayName(const OptionValueType type)
{
    switch (type) {
        case OptionValueType::BOOL:
            return "";
        case OptionValueType::INT:
        case OptionValueType::UINT:
            return "INT";
        case OptionValueType::STRING:
            return "STR";
        case OptionValueType::DIR:
            return "DIR";
        case OptionValueType::FILE:
            return "FILE";
        case OptionValueType::FLOAT:
            return "FLOAT";
        default:
            assert(false);
    }
    return {};  // unreachable
}

}  // namespace

// Some tests are not capabile of calling the printer's ctor that takes an
// explicit column count (i.e. CLI::Run()). Setting this value beforehand provides
// this testing hook. The default of 0 leaves auto-detection enabled.
//
std::size_t HelpMetrics::TestingFixedWidth = 0;

HelpMetrics::HelpMetrics(const Interface& interface, HiddenOptionMode hiddenOptionMode)
    : ShowHiddenOptions{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    MaxColumn = AdjustedMaxColumn(MaxColumn);
    Calculate(interface);
}

HelpMetrics::HelpMetrics(const MultiToolInterface& interface, HiddenOptionMode hiddenOptionMode)
    : ShowHiddenOptions{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    MaxColumn = AdjustedMaxColumn(MaxColumn);
    Calculate(interface);
}

HelpMetrics::HelpMetrics(const Interface& interface, const std::size_t explicitMaxColumn,
                         HiddenOptionMode hiddenOptionMode)
    : MaxColumn{explicitMaxColumn}, ShowHiddenOptions{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    Calculate(interface);
}

HelpMetrics::HelpMetrics(const MultiToolInterface& interface, const std::size_t explicitMaxColumn,
                         HiddenOptionMode hiddenOptionMode)
    : MaxColumn{explicitMaxColumn}, ShowHiddenOptions{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    Calculate(interface);
}

void HelpMetrics::Calculate(const Interface& interface)
{
    // metrics using client options
    for (const auto& optionGroup : interface.OptionGroups()) {
        for (const auto& option : optionGroup.Options) {
            UpdateForOption(option);
        }
    }

    // metrics with builtin options
    UpdateForOption(interface.HelpOption());
    UpdateForOption(interface.VersionOption());

    if (ShowHiddenOptions) {
        UpdateForOption(interface.AlarmsOption());
        UpdateForOption(interface.ExceptionsPassthroughOption());
        UpdateForOption(interface.ShowAllHelpOption());
    }

    if (interface.NumThreadsOption()) {
        UpdateForOption(*interface.NumThreadsOption());
    }
    if (interface.LogLevelOption()) {
        UpdateForOption(*interface.LogLevelOption());
    }
    if (interface.LogFileOption()) {
        UpdateForOption(*interface.LogFileOption());
    }
    if (interface.VerboseOption()) {
        UpdateForOption(*interface.VerboseOption());
    }

    // metrics using pos args
    for (const auto& posArg : interface.PositionalArguments()) {
        auto posArgDisplayText = DisplayName(posArg.Type);
        MaxNameLength = std::max(MaxNameLength, posArg.Name.size());
        MaxTypeLength = std::max(MaxTypeLength, posArgDisplayText.size());
        FormattedPosArgNames.emplace(posArg,
                                     FormattedEntry{posArg.Name, std::move(posArgDisplayText)});
    }
}

void HelpMetrics::Calculate(const MultiToolInterface& interface)
{
    UpdateForOption(interface.HelpOption());
    UpdateForOption(interface.VersionOption());
    if (ShowHiddenOptions) {
        UpdateForOption(interface.ShowAllHelpOption());
    }
}

std::string HelpMetrics::HelpEntry(const OptionData& option) const
{
    const auto& formattedOption = FormattedOptionNames.at(option);
    return HelpEntry(formattedOption.nameString, formattedOption.typeString, option.Description);
}

std::string HelpMetrics::HelpEntry(std::string name, std::string type,
                                   const std::string& description) const
{
    // 2 spaces between left edge and text, and between longest option name
    // and (all) descriptions
    const std::string spacer{"  "};

    std::ostringstream out;

    // formatted name & type
    name.resize(MaxNameLength, ' ');
    type.resize(MaxTypeLength, ' ');
    out << spacer << std::setw(MaxNameLength) << std::left << name << spacer << type << spacer;

    // maybe wrap description
    const auto indent = out.str().length();
    const std::size_t max = MaxColumn - indent;
    const auto wrappedLines = PacBio::Utility::WordWrappedLines(description, max);
    if (!wrappedLines.empty()) {
        out << wrappedLines.at(0);
        for (std::size_t i = 1; i < wrappedLines.size(); ++i) {
            out << '\n' << std::string(indent, ' ') << wrappedLines.at(i);
        }
    }

    return out.str();
}

std::string HelpMetrics::OptionNames(const OptionData& option)
{
    std::ostringstream optionOutput;
    auto first = true;
    for (const auto& name : option.Names) {

        if (first) {
            first = false;
        } else {
            optionOutput << ",";
        }

        if (name.size() == 1) {
            optionOutput << "-";
        } else {
            optionOutput << "--";
        }

        optionOutput << name;
    }
    return optionOutput.str();
}

void HelpMetrics::UpdateForOption(const OptionData& option)
{
    if (option.IsHidden && !ShowHiddenOptions) {
        return;
    }

    auto optionNamesText = OptionNames(option);
    auto optionDisplayText = DisplayName(option.Type);
    MaxNameLength = std::max(MaxNameLength, optionNamesText.size());
    MaxTypeLength = std::max(MaxTypeLength, optionDisplayText.size());
    FormattedOptionNames.emplace(
        option, FormattedEntry{std::move(optionNamesText), std::move(optionDisplayText)});
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
