#include <pbcopper/cli2/Interface.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

#include <map>
#include <stdexcept>
#include <type_traits>

#include <cassert>

using OptionData = PacBio::CLI_v2::internal::OptionData;
using OptionGroupData = PacBio::CLI_v2::internal::OptionGroupData;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using PositionalArgumentData = PacBio::CLI_v2::internal::PositionalArgumentData;
using PositionalArgumentTranslator = PacBio::CLI_v2::internal::PositionalArgumentTranslator;

namespace PacBio {
namespace CLI_v2 {

Interface::Interface(std::string name, std::string description, std::string version)
    : data_{std::move(name),
            std::move(description),
            std::move(version),
            OptionTranslator::Translate(Builtin::Help),
            OptionTranslator::Translate(Builtin::Version),
            OptionTranslator::Translate(Builtin::NumThreads),
            OptionTranslator::Translate(Builtin::LogFile),
            OptionTranslator::Translate(Builtin::LogLevel),
            OptionTranslator::Translate(Builtin::Alarms),
            OptionTranslator::Translate(Builtin::ExceptionPassthrough),
            OptionTranslator::Translate(Builtin::ShowAllHelp)}
{
    if (data_.AppName.empty()) {
        throw std::runtime_error{
            "[pbcopper] command line interface ERROR: application name must not be empty"};
    }
}

Interface& Interface::AddOption(const Option& option)
{
    if (data_.OptionGroups.empty()) {
        data_.OptionGroups.push_back(OptionGroupData{});
    }
    data_.OptionGroups[0].Options.push_back(OptionTranslator::Translate(option));
    return *this;
}

Interface& Interface::AddOptions(const std::vector<Option>& options)
{
    for (const auto& opt : options) {
        AddOption(opt);
    }
    return *this;
}

Interface& Interface::AddOptionGroup(const std::string& title, const std::vector<Option>& options)
{
    data_.OptionGroups.emplace_back(title, OptionTranslator::Translate(options));
    return *this;
}

Interface& Interface::AddOptionGroup(const OptionGroup& group)
{
    data_.OptionGroups.emplace_back(group.name, OptionTranslator::Translate(group.options));
    return *this;
}

Interface& Interface::AddPositionalArgument(const PositionalArgument& posArg)
{
    data_.PositionalArgs.push_back(PositionalArgumentTranslator::Translate(posArg));
    return *this;
}

Interface& Interface::AddPositionalArguments(const std::vector<PositionalArgument>& posArgs)
{
    for (const auto& arg : posArgs) {
        AddPositionalArgument(arg);
    }
    return *this;
}

const internal::OptionData& Interface::AlarmsOption() const { return data_.AlarmsOption; }

const std::string& Interface::ApplicationDescription() const { return data_.AppDescription; }

const std::string& Interface::ApplicationName() const { return data_.AppName; }

const std::string& Interface::ApplicationVersion() const { return data_.AppVersion; }

Logging::LogLevel Interface::DefaultLogLevel() const
{
    if (data_.LogLevelOption) {
        const auto& value = *data_.LogLevelOption->DefaultValue;
        return Logging::LogLevel{std::get<std::string>(value)};
    } else {
        return data_.LogConfig.Level;
    }
}

Interface& Interface::DefaultLogLevel(Logging::LogLevel level)
{
    // Error if client is setting default log level, when that option has been disabled.
    assert(data_.LogLevelOption);
    data_.LogLevelOption->DefaultValue = level.ToString();
    data_.LogConfig.Level = level;
    return *this;
}

Interface& Interface::DisableLogFileOption()
{
    data_.LogFileOption.reset();
    return *this;
}

Interface& Interface::DisableLogLevelOption()
{
    data_.LogLevelOption.reset();
    return *this;
}

Interface& Interface::DisableNumThreadsOption()
{
    data_.NumThreadsOption.reset();
    return *this;
}

Interface& Interface::EnableVerboseOption()
{
    data_.VerboseOption = OptionTranslator::Translate(Builtin::Verbose);
    return *this;
}

const std::string& Interface::Example() const { return data_.Example; }

Interface& Interface::Example(std::string example)
{
    data_.Example = std::move(example);
    return *this;
}

const internal::OptionData& Interface::ExceptionsPassthroughOption() const
{
    return data_.ExceptionPassthroughOption;
}

bool Interface::HasRequiredPosArgs() const { return NumRequiredPosArgs() > 0; }

const std::string& Interface::HelpFooter() const { return data_.Footer; }

Interface& Interface::HelpFooter(std::string footer)
{
    data_.Footer = std::move(footer);
    return *this;
}

const OptionData& Interface::HelpOption() const { return data_.HelpOption; }

const Logging::LogConfig& Interface::LogConfig() const { return data_.LogConfig; }

Interface& Interface::LogConfig(const Logging::LogConfig& config)
{
    data_.LogConfig = config;
    return *this;
}

const std::optional<internal::OptionData>& Interface::LogFileOption() const
{
    return data_.LogFileOption;
}

const std::optional<internal::OptionData>& Interface::LogLevelOption() const
{
    return data_.LogLevelOption;
}

const std::optional<internal::OptionData>& Interface::NumThreadsOption() const
{
    return data_.NumThreadsOption;
}

Results Interface::MakeDefaultResults() const
{
    Results results;
    const auto options = Options();
    for (const auto& opt : options) {
        if (opt.DefaultValue) {
            results.AddDefaultOption(opt);
        }
    }
    results.PositionalArguments(PositionalArguments());
    return results;
}

size_t Interface::NumRequiredPosArgs() const
{
    size_t count = 0;
    for (const auto& posArg : data_.PositionalArgs) {
        if (posArg.Required) {
            ++count;
        }
    }
    return count;
}

std::vector<OptionData> Interface::Options() const
{
    std::vector<OptionData> result;

    // add registered objects
    for (const auto& group : data_.OptionGroups) {
        for (const auto& opt : group.Options) {
            result.push_back(opt);
        }
    }

    // add builtins
    result.push_back(data_.HelpOption);
    result.push_back(data_.VersionOption);
    result.push_back(data_.AlarmsOption);
    result.push_back(data_.ExceptionPassthroughOption);
    result.push_back(data_.ShowAllHelpOption);
    if (data_.NumThreadsOption) {
        result.push_back(*data_.NumThreadsOption);
    }
    if (data_.LogLevelOption) {
        result.push_back(*data_.LogLevelOption);
    }
    if (data_.LogFileOption) {
        result.push_back(*data_.LogFileOption);
    }
    if (data_.VerboseOption) {
        result.push_back(*data_.VerboseOption);
    }

    return result;
}

const std::vector<OptionGroupData>& Interface::OptionGroups() const { return data_.OptionGroups; }

const std::vector<PositionalArgumentData>& Interface::PositionalArguments() const
{
    return data_.PositionalArgs;
}

void Interface::PrintVersion() const { data_.VersionPrinter(*this); }

Interface& Interface::RegisterVersionPrinter(VersionPrinterCallback printer)
{
    data_.VersionPrinter = printer;
    return *this;
}

const internal::OptionData& Interface::ShowAllHelpOption() const { return data_.ShowAllHelpOption; }

const std::optional<internal::OptionData>& Interface::VerboseOption() const
{
    return data_.VerboseOption;
}

const internal::OptionData& Interface::VersionOption() const { return data_.VersionOption; }

}  // namespace CLI_v2
}  // namespace PacBio
