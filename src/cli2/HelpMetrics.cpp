#include <pbcopper/cli2/internal/HelpMetrics.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <cassert>

#include <algorithm>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

#include <pbcopper/cli2/OptionValue.h>
#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

namespace {

size_t AdjustedMaxColumn(size_t maxColumn)
{
    // if column count is explicitly set (i.e. for testing), use that
    if (HelpMetrics::TestingFixedWidth != 0) return HelpMetrics::TestingFixedWidth;

    // otherwise determine column count from terminal width (default behavior)
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    if (ws.ws_col >= 2) maxColumn = ws.ws_col - 1;
    constexpr const size_t MaxColumn = 119;
    return std::min(maxColumn, MaxColumn);
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
size_t HelpMetrics::TestingFixedWidth = 0;

HelpMetrics::HelpMetrics(const Interface& interface)
{
    maxColumn = AdjustedMaxColumn(maxColumn);
    Calculate(interface);
}

HelpMetrics::HelpMetrics(const MultiToolInterface& interface)
{
    maxColumn = AdjustedMaxColumn(maxColumn);
    Calculate(interface);
}

HelpMetrics::HelpMetrics(const Interface& interface, const size_t explicitMaxColumn)
    : maxColumn{explicitMaxColumn}
{
    Calculate(interface);
}

HelpMetrics::HelpMetrics(const MultiToolInterface& interface, const size_t explicitMaxColumn)
    : maxColumn{explicitMaxColumn}
{
    Calculate(interface);
}

void HelpMetrics::Calculate(const Interface& interface)
{
    // metrics using client options
    for (const auto& optionGroup : interface.OptionGroups()) {
        for (const auto& option : optionGroup.options)
            UpdateForOption(option);
    }

    // metrics with builtin options
    UpdateForOption(interface.HelpOption());
    UpdateForOption(interface.VersionOption());
    if (interface.NumThreadsOption()) UpdateForOption(interface.NumThreadsOption().get());
    if (interface.LogLevelOption()) UpdateForOption(interface.LogLevelOption().get());
    if (interface.LogFileOption()) UpdateForOption(interface.LogFileOption().get());
    if (interface.VerboseOption()) UpdateForOption(interface.VerboseOption().get());

    // metrics using pos args
    for (const auto& posArg : interface.PositionalArguments()) {
        auto posArgDisplayText = DisplayName(posArg.type);
        maxNameLength = std::max(maxNameLength, posArg.name.size());
        maxTypeLength = std::max(maxTypeLength, posArgDisplayText.size());
        formattedPosArgNames.emplace(posArg,
                                     FormattedEntry{posArg.name, std::move(posArgDisplayText)});
    }
}

void HelpMetrics::Calculate(const MultiToolInterface& interface)
{
    UpdateForOption(interface.HelpOption());
    UpdateForOption(interface.VersionOption());
}

std::string HelpMetrics::HelpEntry(const OptionData& option) const
{
    const auto& formattedOption = formattedOptionNames.at(option);
    return HelpEntry(formattedOption.nameString, formattedOption.typeString, option.description);
}

std::string HelpMetrics::HelpEntry(std::string name, std::string type,
                                   const std::string& description) const
{
    // 2 spaces between left edge and text, and between longest option name
    // and (all) descriptions
    const std::string spacer{"  "};

    std::ostringstream out;

    // formatted name & type
    name.resize(maxNameLength, ' ');
    type.resize(maxTypeLength, ' ');
    out << spacer << std::setw(maxNameLength) << std::left << name << spacer << type << spacer;

    // maybe wrap description
    const auto indent = out.str().length();
    const size_t max = maxColumn - indent;
    const auto wrappedLines = PacBio::Utility::WordWrappedLines(description, max);
    if (!wrappedLines.empty()) {
        out << wrappedLines.at(0);
        for (size_t i = 1; i < wrappedLines.size(); ++i)
            out << '\n' << std::string(indent, ' ') << wrappedLines.at(i);
    }

    return out.str();
}

std::string HelpMetrics::OptionNames(const OptionData& option)
{
    if (option.isHidden) return {};

    std::ostringstream optionOutput;
    auto first = true;
    for (const auto& name : option.names) {

        if (first)
            first = false;
        else
            optionOutput << ",";

        if (name.size() == 1)
            optionOutput << "-";
        else
            optionOutput << "--";

        optionOutput << name;
    }
    return optionOutput.str();
}

void HelpMetrics::UpdateForOption(const OptionData& option)
{
    if (option.isHidden) return;

    auto optionNamesText = OptionNames(option);
    auto optionDisplayText = DisplayName(option.type);
    maxNameLength = std::max(maxNameLength, optionNamesText.size());
    maxTypeLength = std::max(maxTypeLength, optionDisplayText.size());
    formattedOptionNames.emplace(
        option, FormattedEntry{std::move(optionNamesText), std::move(optionDisplayText)});
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
