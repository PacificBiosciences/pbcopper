#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>

#include <algorithm>
#include <ostream>
#include <sstream>

#include <pbcopper/utility/StringUtils.h>

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

InterfaceHelpPrinter::InterfaceHelpPrinter(Interface interface)
    : metrics_{interface}, interface_{std::move(interface)}
{
    MakeHelpText();
}

InterfaceHelpPrinter::InterfaceHelpPrinter(Interface interface, const size_t maxColumn)
    : metrics_{interface, maxColumn}, interface_{std::move(interface)}
{
    MakeHelpText();
}

std::string InterfaceHelpPrinter::Choices(const OptionData& option)
{
    if (option.isHidden || option.choices.empty() || option.isChoicesHidden) return {};

    std::ostringstream out;
    for (const auto& choice : option.choices) {
        switch (option.type) {
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
    if (!ShouldShowDefaultValue(option)) return {};

    std::ostringstream out;
    switch (option.type) {
        case OptionValueType::INT:
            out << OptionValueToInt(option.defaultValue.get());
            break;
        case OptionValueType::UINT:
            out << OptionValueToUInt(option.defaultValue.get());
            break;
        case OptionValueType::FLOAT:
            out << OptionValueToDouble(option.defaultValue.get());
            break;
        case OptionValueType::STRING:
        case OptionValueType::FILE:
        case OptionValueType::DIR:
            out << OptionValueToString(option.defaultValue.get());
            break;
        case OptionValueType::BOOL: {
            const bool on = OptionValueToBool(option.defaultValue.get());
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
    if (description.empty()) return {};

    // maybe word-wrap description
    std::ostringstream out;
    const auto indent = interface_.ApplicationName().size() + 3;  // " - " spacer
    const auto max = metrics_.maxColumn - indent;
    const auto wrappedLines = Utility::WordWrappedLines(description, max);
    if (!wrappedLines.empty()) {
        out << wrappedLines.at(0);
        for (size_t i = 1; i < wrappedLines.size(); ++i)
            out << '\n' << std::string(indent, ' ') << wrappedLines.at(i);
    }
    return out.str();
}

void InterfaceHelpPrinter::MakeHelpText()
{
    std::ostringstream result;
    result << interface_.ApplicationName();

    const std::string description = Description();
    if (!description.empty()) result << " - " << Description();

    result << '\n' << '\n' << Usage() << '\n';

    const std::string posArgs = PositionalArguments();
    if (!posArgs.empty()) result << '\n' << posArgs;  // << '\n';

    const std::string options = Options();
    if (!options.empty()) result << '\n' << options << '\n';

    const auto& footer = interface_.HelpFooter();
    if (!footer.empty()) result << footer << '\n';

    text_ = result.str();
}

const HelpMetrics& InterfaceHelpPrinter::Metrics() { return metrics_; }

std::string InterfaceHelpPrinter::Option(const OptionData& option)
{
    const auto& formattedOption = metrics_.formattedOptionNames.at(option);
    return metrics_.HelpEntry(formattedOption.nameString, formattedOption.typeString,
                              OptionDescription(option));
}

std::string InterfaceHelpPrinter::OptionDescription(const OptionData& option)
{
    // layout full description, with optional components
    std::ostringstream out;
    out << option.description;

    const std::string choices = Choices(option);
    if (!choices.empty()) out << " Valid choices: (" << choices << ").";

    const auto defaultValue = DefaultValue(option);
    if (!defaultValue.empty()) out << " [" << defaultValue << ']';

    return out.str();
}

std::string InterfaceHelpPrinter::OptionGroup(const OptionGroupData& group)
{
    if (group.options.empty()) return {};

    std::ostringstream out;
    if (!group.name.empty()) out << group.name << ":\n";

    for (const OptionData& option : group.options) {
        if (!option.isHidden) out << Option(option) << '\n';
    }
    return out.str();
}

std::string InterfaceHelpPrinter::Options()
{
    // print client option groups
    std::ostringstream out;
    const auto& optionGroups = interface_.OptionGroups();
    for (const auto& optionGroup : optionGroups)
        out << OptionGroup(optionGroup) << '\n';

    // print builtin group
    OptionGroupData group;
    if (optionGroups.empty()) group.name = "Options";
    group.options.push_back(interface_.HelpOption());
    group.options.push_back(interface_.VersionOption());
    if (interface_.NumThreadsOption()) group.options.push_back(interface_.NumThreadsOption().get());
    if (interface_.LogLevelOption()) group.options.push_back(interface_.LogLevelOption().get());
    if (interface_.LogFileOption()) group.options.push_back(interface_.LogFileOption().get());
    if (interface_.VerboseOption()) group.options.push_back(interface_.VerboseOption().get());
    out << OptionGroup(group);
    return out.str();
}

std::string InterfaceHelpPrinter::PositionalArgument(const PositionalArgumentData& posArg)
{
    const auto& formattedPosArg = metrics_.formattedPosArgNames.at(posArg);
    return metrics_.HelpEntry(formattedPosArg.nameString, formattedPosArg.typeString,
                              posArg.description);
}

std::string InterfaceHelpPrinter::PositionalArguments()
{
    const auto& posArgs = interface_.PositionalArguments();
    if (posArgs.empty()) return {};

    std::ostringstream out;
    for (const auto& posArg : posArgs)
        out << PositionalArgument(posArg) << '\n';

    return out.str();
}

void InterfaceHelpPrinter::Print(std::ostream& out) const { out << text_; }

bool InterfaceHelpPrinter::ShouldShowDefaultValue(const OptionData& option)
{
    // omit if switch OR null
    if (option.type == OptionValueType::BOOL) return false;

    // omit if string-type option has an empty default
    if (option.type == OptionValueType::STRING) {
        const auto& defaultValue = OptionValueToString(option.defaultValue.get());
        return !defaultValue.empty();
    }

    // otherwise (maybe) use default value
    return option.defaultValue.is_initialized();
}

std::string InterfaceHelpPrinter::Usage()
{
    std::ostringstream usage;

    usage << "Usage:\n"
          << "  " << interface_.ApplicationName() << " [options]";

    // print all required pos args & then optional ones after, regardless of order added
    for (const auto& posArg : interface_.PositionalArguments()) {
        if (posArg.required) usage << " <" << posArg.name << '>';
    }
    for (const auto& posArg : interface_.PositionalArguments()) {
        if (!posArg.required) usage << " [" << posArg.name << ']';
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
