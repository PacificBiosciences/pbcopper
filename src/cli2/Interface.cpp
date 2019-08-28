#include <pbcopper/cli2/Interface.h>

#include <cassert>
#include <map>
#include <stdexcept>
#include <type_traits>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

using OptionData = PacBio::CLI_v2::internal::OptionData;
using OptionGroupData = PacBio::CLI_v2::internal::OptionGroupData;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using PositionalArgumentData = PacBio::CLI_v2::internal::PositionalArgumentData;
using PositionalArgumentTranslator = PacBio::CLI_v2::internal::PositionalArgumentTranslator;

namespace PacBio {
namespace CLI_v2 {

static_assert(std::is_copy_constructible<Interface>::value,
              "Interface(const Interface&) is not = default");
static_assert(std::is_copy_assignable<Interface>::value,
              "Interface& operator=(const Interface&) is not = default");

static_assert(std::is_nothrow_move_constructible<Interface>::value,
              "Interface(Interface&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Interface>::value ==
                  std::is_nothrow_move_assignable<internal::InterfaceData>::value,
              "");

Interface::Interface(std::string name, std::string description, std::string version)
    : data_{std::move(name),
            std::move(description),
            std::move(version),
            OptionTranslator::Translate(Builtin::Help),
            OptionTranslator::Translate(Builtin::Version),
            OptionTranslator::Translate(Builtin::NumThreads),
            OptionTranslator::Translate(Builtin::LogFile),
            OptionTranslator::Translate(Builtin::LogLevel)}
{
    if (data_.appName_.empty()) {
        throw std::runtime_error{
            "[pbcopper] command line interface ERROR: application name must not be empty"};
    }
}

Interface& Interface::AddOption(const Option& option)
{
    if (data_.optionGroups_.empty()) data_.optionGroups_.push_back(OptionGroupData{});
    data_.optionGroups_[0].options.push_back(OptionTranslator::Translate(option));
    return *this;
}

Interface& Interface::AddOptions(const std::vector<Option>& options)
{
    for (const auto& opt : options)
        AddOption(opt);
    return *this;
}

Interface& Interface::AddOptionGroup(const std::string& title, const std::vector<Option>& options)
{
    data_.optionGroups_.emplace_back(title, OptionTranslator::Translate(options));
    return *this;
}

Interface& Interface::AddOptionGroup(const OptionGroup& group)
{
    data_.optionGroups_.emplace_back(group.name, OptionTranslator::Translate(group.options));
    return *this;
}

Interface& Interface::AddPositionalArgument(const PositionalArgument& posArg)
{
    data_.positionalArgs_.push_back(PositionalArgumentTranslator::Translate(posArg));
    return *this;
}

Interface& Interface::AddPositionalArguments(const std::vector<PositionalArgument>& posArgs)
{
    for (const auto& arg : posArgs)
        AddPositionalArgument(arg);
    return *this;
}

const std::string& Interface::ApplicationDescription() const { return data_.appDescription_; }

const std::string& Interface::ApplicationName() const { return data_.appName_; }

const std::string& Interface::ApplicationVersion() const { return data_.appVersion_; }

Logging::LogLevel Interface::DefaultLogLevel() const
{
    if (data_.logLevelOption_) {
        const auto& value = data_.logLevelOption_->defaultValue.get();
        return Logging::LogLevel{boost::get<std::string>(value)};
    } else
        return data_.logConfig_.Level;
}

Interface& Interface::DefaultLogLevel(Logging::LogLevel level)
{
    // Error if client is setting default log level, when that option has been disabled.
    assert(data_.logLevelOption_);
    data_.logLevelOption_->defaultValue = level.ToString();
    return *this;
}

Interface& Interface::DisableLogFileOption()
{
    data_.logFileOption_ = boost::none;
    return *this;
}

Interface& Interface::DisableLogLevelOption()
{
    data_.logLevelOption_ = boost::none;
    return *this;
}

Interface& Interface::DisableNumThreadsOption()
{
    data_.numThreadsOption_ = boost::none;
    return *this;
}

Interface& Interface::EnableVerboseOption()
{
    data_.verboseOption_ = OptionTranslator::Translate(Builtin::Verbose);
    return *this;
}

const std::string& Interface::Example() const { return data_.example_; }

Interface& Interface::Example(std::string example)
{
    data_.example_ = std::move(example);
    return *this;
}

bool Interface::HasRequiredPosArgs() const
{
    for (const auto& posArg : data_.positionalArgs_) {
        if (posArg.required) return true;
    }
    return false;
}

const std::string& Interface::HelpFooter() const { return data_.footer_; }

Interface& Interface::HelpFooter(std::string footer)
{
    data_.footer_ = std::move(footer);
    return *this;
}

const OptionData& Interface::HelpOption() const { return data_.helpOption_; }

const Logging::LogConfig& Interface::LogConfig() const { return data_.logConfig_; }

Interface& Interface::LogConfig(const Logging::LogConfig& config)
{
    data_.logConfig_ = config;
    return *this;
}

const boost::optional<internal::OptionData>& Interface::LogFileOption() const
{
    return data_.logFileOption_;
}

const boost::optional<internal::OptionData>& Interface::LogLevelOption() const
{
    return data_.logLevelOption_;
}

const boost::optional<internal::OptionData>& Interface::NumThreadsOption() const
{
    return data_.numThreadsOption_;
}

Results Interface::MakeDefaultResults() const
{
    Results results;
    const auto options = Options();
    for (const auto& opt : options) {
        if (opt.defaultValue.is_initialized()) results.AddDefaultOption(opt);
    }
    results.PositionalArguments(PositionalArguments());
    return results;
}

std::vector<OptionData> Interface::Options() const
{
    std::vector<OptionData> result;

    // add registered objects
    for (const auto& group : data_.optionGroups_) {
        for (const auto& opt : group.options)
            result.push_back(opt);
    }

    // add builtins
    result.push_back(data_.helpOption_);
    result.push_back(data_.versionOption_);
    if (data_.numThreadsOption_) result.push_back(data_.numThreadsOption_.get());
    if (data_.logLevelOption_) result.push_back(data_.logLevelOption_.get());
    if (data_.logFileOption_) result.push_back(data_.logFileOption_.get());
    if (data_.verboseOption_) result.push_back(data_.verboseOption_.get());
    return result;
}

const std::vector<OptionGroupData>& Interface::OptionGroups() const { return data_.optionGroups_; }

const std::vector<PositionalArgumentData>& Interface::PositionalArguments() const
{
    return data_.positionalArgs_;
}

const boost::optional<internal::OptionData>& Interface::VerboseOption() const
{
    return data_.verboseOption_;
}

const internal::OptionData& Interface::VersionOption() const { return data_.versionOption_; }

}  // namespace CLI_v2
}  // namespace PacBio
