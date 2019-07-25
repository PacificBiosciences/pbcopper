#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>

#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>
#include <pbcopper/json/JSON.h>
#include <pbcopper/utility/StringUtils.h>

using Option = PacBio::CLI_v2::Option;
using OptionData = PacBio::CLI_v2::internal::OptionData;
using OptionValue = PacBio::CLI_v2::OptionValue;
using OptionValueType = PacBio::CLI_v2::OptionValueType;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;
using PositionalArgumentData = PacBio::CLI_v2::internal::PositionalArgumentData;
using PositionalArgumentTranslator = PacBio::CLI_v2::internal::PositionalArgumentTranslator;

namespace {

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

namespace PacBio {
namespace CLI_v2 {
namespace internal {

// Some tests are not capabile of calling the printer's ctor that takes an
// explicit column count (i.e. CLI::Run()). Setting this value beforehand provides
// this testing hook. The default of 0 leaves auto-detection enabled.
//
size_t InterfaceHelpPrinter::TestingFixedWidth = 0;

InterfaceHelpPrinter::InterfaceHelpPrinter(Interface interface) : interface_{std::move(interface)}
{
    if (TestingFixedWidth == 0) {
        // determine column count from terminal width (default behavior)
        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        if (ws.ws_col >= 2) metrics_.maxColumn = ws.ws_col - 1;
        constexpr const size_t MaxColumn = 119;
        metrics_.maxColumn = std::min(metrics_.maxColumn, MaxColumn);
    } else {
        // use provided column count (testing only)
        metrics_.maxColumn = TestingFixedWidth;
    }

    CalculateMetrics();
    MakeHelpText();
}

InterfaceHelpPrinter::InterfaceHelpPrinter(Interface interface, const size_t maxColumn)
    : interface_{std::move(interface)}
{
    // use client-provided column count
    metrics_.maxColumn = maxColumn;

    CalculateMetrics();
    MakeHelpText();
}

void InterfaceHelpPrinter::CalculateMetrics()
{
    // metrics using client options
    auto updateMetricsWithOption = [this](const OptionData& option) {
        if (option.isHidden) return;

        auto optionNamesText = OptionNames(option);
        auto optionDisplayText = DisplayName(option.type);
        metrics_.maxNameLength = std::max(metrics_.maxNameLength, optionNamesText.size());
        metrics_.maxTypeLength = std::max(metrics_.maxTypeLength, optionDisplayText.size());
        metrics_.formattedOptionNames.emplace(
            option, FormattedEntry{std::move(optionNamesText), std::move(optionDisplayText)});
    };

    for (const auto& optionGroup : interface_.OptionGroups()) {
        for (const auto& option : optionGroup.options)
            updateMetricsWithOption(option);
    }

    // metrics with builtin options
    updateMetricsWithOption(interface_.HelpOption());
    updateMetricsWithOption(interface_.LogLevelOption());
    updateMetricsWithOption(interface_.LogFileOption());
    updateMetricsWithOption(interface_.NumThreadsOption());
    updateMetricsWithOption(interface_.VersionOption());

    // metrics using pos args
    for (const auto& posArg : interface_.PositionalArguments()) {
        auto posArgDisplayText = DisplayName(posArg.type);
        metrics_.maxNameLength = std::max(metrics_.maxNameLength, posArg.name.size());
        metrics_.maxTypeLength = std::max(metrics_.maxTypeLength, posArgDisplayText.size());
        metrics_.formattedPosArgNames.emplace(
            posArg, FormattedEntry{posArg.name, std::move(posArgDisplayText)});
    }
}

std::string InterfaceHelpPrinter::Choices(const OptionData& option)
{
    if (option.isHidden || option.choices.empty() || option.isChoicesHidden) return {};

    std::ostringstream out;
    for (const auto& choice : option.choices) {
        switch (option.type) {
            case OptionValueType::INT:
                out << boost::get<int>(choice);
                break;
            case OptionValueType::UINT:
                out << boost::get<unsigned int>(choice);
                break;
            case OptionValueType::FLOAT:
                out << boost::get<double>(choice);
                break;
            case OptionValueType::STRING:
            case OptionValueType::FILE:
            case OptionValueType::DIR:
                out << boost::get<std::string>(choice);
                break;
            case OptionValueType::BOOL:
                out << (boost::get<bool>(choice) ? "true" : "false");
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

    std::stringstream out;
    switch (option.type) {
        case OptionValueType::INT:
            out << boost::get<int>(option.defaultValue.get());
            break;
        case OptionValueType::UINT:
            out << boost::get<unsigned int>(option.defaultValue.get());
            break;
        case OptionValueType::FLOAT:
            out << boost::get<double>(option.defaultValue.get());
            break;
        case OptionValueType::STRING:
        case OptionValueType::FILE:
        case OptionValueType::DIR:
            out << boost::get<std::string>(option.defaultValue.get());
            break;
        case OptionValueType::BOOL: {
            const bool on = boost::get<bool>(option.defaultValue.get());
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

std::string InterfaceHelpPrinter::HelpEntry(std::string name, std::string type,
                                            const std::string& description)
{
    // 2 spaces between left edge and text, and between longest option name
    // and (all) descriptions
    const std::string spacer{"  "};

    std::ostringstream out;

    // formatted name & type
    name.resize(metrics_.maxNameLength, ' ');
    type.resize(metrics_.maxTypeLength, ' ');
    out << spacer << std::setw(metrics_.maxNameLength) << std::left << name << spacer << type
        << spacer;

    // maybe wrap description
    const auto indent = out.str().length();
    const size_t max = metrics_.maxColumn - indent;
    const auto wrappedLines = PacBio::Utility::WordWrappedLines(description, max);
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
    return HelpEntry(formattedOption.nameString, formattedOption.typeString,
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

std::string InterfaceHelpPrinter::OptionNames(const OptionData& option)
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
    group.options = {interface_.HelpOption(), interface_.LogLevelOption(),
                     interface_.LogFileOption(), interface_.NumThreadsOption(),
                     interface_.VersionOption()};
    out << OptionGroup(group);
    return out.str();
}

std::string InterfaceHelpPrinter::PositionalArgument(const PositionalArgumentData& posArg)
{
    const auto& formattedPosArg = metrics_.formattedPosArgNames.at(posArg);
    return HelpEntry(formattedPosArg.nameString, formattedPosArg.typeString, posArg.description);
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
        const auto& defaultValue = boost::get<std::string>(option.defaultValue.get());
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
