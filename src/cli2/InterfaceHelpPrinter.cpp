#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>

#include <pbcopper/utility/StringUtils.h>
#include "PbBoilerplateDisclaimer.h"

#include <algorithm>
#include <ostream>
#include <sstream>

using HelpMetrics = PacBio::CLI_v2::internal::HelpMetrics;
using Option = PacBio::CLI_v2::Option;
using OptionData = PacBio::CLI_v2::internal::OptionData;
using OptionValue = PacBio::CLI_v2::OptionValue;
using OptionValueType = PacBio::CLI_v2::OptionValueType;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;
using PositionalArgumentData = PacBio::CLI_v2::internal::PositionalArgumentData;

namespace PacBio {
namespace CLI_v2 {
namespace internal {

InterfaceHelpPrinter::InterfaceHelpPrinter(Interface interface, HiddenOptionMode hiddenOptionMode)
    : metrics_{interface, hiddenOptionMode}
    , interface_{std::move(interface)}
    , showHiddenOptions_{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    MakeHelpText();
}

InterfaceHelpPrinter::InterfaceHelpPrinter(Interface interface, const std::size_t maxColumn,
                                           HiddenOptionMode hiddenOptionMode)
    : metrics_{interface, maxColumn, hiddenOptionMode}
    , interface_{std::move(interface)}
    , showHiddenOptions_{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    MakeHelpText();
}

std::string InterfaceHelpPrinter::Choices(const OptionData& option)
{
    // nothing to show
    if (option.Choices.empty()) {
        return {};
    }

    // check for hidden status
    if ((option.IsHidden || option.IsChoicesHidden) && !showHiddenOptions_) {
        return {};
    }

    std::ostringstream out;
    for (const auto& choice : option.Choices) {
        switch (option.Type) {
            case OptionValueType::INT:
                out << OptionValueToInt(choice);
                break;
            case OptionValueType::UINT:
                out << OptionValueToUInt(choice);
                break;
            case OptionValueType::FLOAT:
                out << OptionValueToDouble(choice);
                break;
            case OptionValueType::STRING:
            case OptionValueType::FILE:
            case OptionValueType::DIR:
                out << OptionValueToString(choice);
                break;
            case OptionValueType::BOOL:
                out << (OptionValueToBool(choice) ? "true" : "false");
                break;
        }
        out << ", ";
    }

    std::string result = out.str();
    result.erase(result.size() - 2, 2);  // trim trailing ", "
    return result;
}

std::string InterfaceHelpPrinter::DefaultValue(const OptionData& option)
{
    if (!ShouldShowDefaultValue(option)) {
        return {};
    }

    std::ostringstream out;
    switch (option.Type) {
        case OptionValueType::INT:
            out << OptionValueToInt(*option.DefaultValue);
            break;
        case OptionValueType::UINT:
            out << OptionValueToUInt(*option.DefaultValue);
            break;
        case OptionValueType::FLOAT:
            out << OptionValueToDouble(*option.DefaultValue);
            break;
        case OptionValueType::STRING:
        case OptionValueType::FILE:
        case OptionValueType::DIR:
            out << OptionValueToString(*option.DefaultValue);
            break;
        case OptionValueType::BOOL: {
            const bool on = OptionValueToBool(*option.DefaultValue);
            out << (on ? "true" : "false");
            break;
        }
        default:
            assert(false);
    }
    return out.str();
}

std::string InterfaceHelpPrinter::Description()
{
    const auto& description = interface_.ApplicationDescription();
    if (description.empty()) {
        return {};
    }

    // maybe word-wrap description
    std::ostringstream out;
    const auto indent = interface_.ApplicationName().size() + 3;  // " - " spacer
    const auto max = metrics_.MaxColumn - indent;
    const auto wrappedLines = Utility::WordWrappedLines(description, max);
    if (!wrappedLines.empty()) {
        out << wrappedLines.at(0);
        for (std::size_t i = 1; i < wrappedLines.size(); ++i) {
            out << '\n' << std::string(indent, ' ') << wrappedLines.at(i);
        }
    }
    return out.str();
}

void InterfaceHelpPrinter::MakeHelpText()
{
    std::ostringstream result;
    result << interface_.ApplicationName();

    const std::string description = Description();
    if (!description.empty()) {
        result << " - " << Description();
    }

    result << '\n' << '\n' << Usage() << '\n';

    const std::string posArgs = PositionalArguments();
    if (!posArgs.empty()) {
        result << '\n' << posArgs;
    }

    const std::string options = Options();
    if (!options.empty()) {
        result << '\n' << options << '\n';
    }

    const auto& footer = interface_.HelpFooter();
    if (!footer.empty()) {
        result << footer << '\n';

        // Some application footers contain trailing newlines, some do not.
        // Ensure we have at least one empty line between footer and disclaimer.
        if (footer.back() != '\n') {
            result << '\n';
        }
    }

    result << PB_BOILERPLATE_DISCLAIMER;

    text_ = result.str();
}

const HelpMetrics& InterfaceHelpPrinter::Metrics() { return metrics_; }

std::string InterfaceHelpPrinter::Option(const OptionData& option)
{
    const auto& formattedOption = metrics_.FormattedOptionNames.at(option);
    return metrics_.HelpEntry(formattedOption.nameString, formattedOption.typeString,
                              OptionDescription(option));
}

std::string InterfaceHelpPrinter::OptionDescription(const OptionData& option)
{
    // layout full description, with optional components
    std::ostringstream out;
    out << option.Description;

    const std::string choices = Choices(option);
    if (!choices.empty()) {
        out << " Valid choices: (" << choices << ").";
    }

    const auto defaultValue = DefaultValue(option);
    if (!defaultValue.empty() && !option.IsDefaultValueHidden) {
        out << " [" << defaultValue << ']';
    }

    return out.str();
}

std::string InterfaceHelpPrinter::OptionGroup(const OptionGroupData& group)
{
    if (group.Options.empty()) {
        return {};
    }

    std::ostringstream out;
    if (!group.Name.empty()) {
        out << group.Name << ":\n";
    }

    for (const OptionData& option : group.Options) {
        if (!option.IsHidden || showHiddenOptions_) {
            out << Option(option) << '\n';
        }
    }
    return out.str();
}

std::string InterfaceHelpPrinter::Options()
{
    // print client option groups
    std::ostringstream out;
    const auto& optionGroups = interface_.OptionGroups();
    for (const auto& optionGroup : optionGroups) {
        out << OptionGroup(optionGroup) << '\n';
    }

    // print builtin group
    OptionGroupData group;
    if (optionGroups.empty()) {
        group.Name = "Options";
    }
    group.Options.push_back(interface_.HelpOption());
    group.Options.push_back(interface_.VersionOption());
    group.Options.push_back(interface_.AlarmsOption());
    group.Options.push_back(interface_.ExceptionsPassthroughOption());
    group.Options.push_back(interface_.ShowAllHelpOption());

    if (interface_.NumThreadsOption()) {
        group.Options.push_back(*interface_.NumThreadsOption());
    }
    if (interface_.LogLevelOption()) {
        group.Options.push_back(*interface_.LogLevelOption());
    }
    if (interface_.LogFileOption()) {
        group.Options.push_back(*interface_.LogFileOption());
    }
    if (interface_.VerboseOption()) {
        group.Options.push_back(*interface_.VerboseOption());
    }
    out << OptionGroup(group);
    return out.str();
}

std::string InterfaceHelpPrinter::PositionalArgument(const PositionalArgumentData& posArg)
{
    const auto& formattedPosArg = metrics_.FormattedPosArgNames.at(posArg);
    return metrics_.HelpEntry(formattedPosArg.nameString, formattedPosArg.typeString,
                              posArg.Description);
}

std::string InterfaceHelpPrinter::PositionalArguments()
{
    const auto& posArgs = interface_.PositionalArguments();
    if (posArgs.empty()) {
        return {};
    }

    std::ostringstream out;
    for (const auto& posArg : posArgs) {
        out << PositionalArgument(posArg) << '\n';
    }

    return out.str();
}

void InterfaceHelpPrinter::Print(std::ostream& out) const { out << text_; }

bool InterfaceHelpPrinter::ShouldShowDefaultValue(const OptionData& option)
{
    // omit if switch OR null
    if (option.Type == OptionValueType::BOOL) {
        return false;
    }

    // omit if string-type option has an empty default
    if (option.Type == OptionValueType::STRING) {
        const auto& defaultValue = OptionValueToString(*option.DefaultValue);
        return !defaultValue.empty();
    }

    // otherwise (maybe) use default value
    return bool{option.DefaultValue};
}

std::string InterfaceHelpPrinter::Usage()
{
    std::ostringstream usage;

    usage << "Usage:\n"
          << "  " << interface_.ApplicationName() << " [options]";

    // print all required pos args & then optional ones after, regardless of order added
    for (const auto& posArg : interface_.PositionalArguments()) {
        if (posArg.Required) {
            usage << " <" << posArg.Name << '>';
        }
    }
    for (const auto& posArg : interface_.PositionalArguments()) {
        if (!posArg.Required) {
            usage << " [" << posArg.Name << ']';
        }
    }

    return usage.str();
}

std::ostream& operator<<(std::ostream& out, const InterfaceHelpPrinter& printer)
{
    printer.Print(out);
    return out;
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
