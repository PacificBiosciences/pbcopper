// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_INTERFACE_H
#define PBCOPPER_CLI_v2_INTERFACE_H

#include <pbcopper/PbcopperConfig.h>

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/OptionGroup.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/InterfaceData.h>
#include <pbcopper/logging/Logging.h>

namespace PacBio {
namespace CLI_v2 {

///
/// The main entry point for defining an application's CLI. The Interface provides
/// the info needed for printing help/version and defines all options and positional
/// arguments.
///
class Interface
{
public:
    Interface(std::string name, std::string description = std::string{},
              std::string version = std::string{});

public:
    ///
    /// Add an option to the interface.
    ///
    /// The option will be added to the default group.
    ///
    Interface& AddOption(const Option& option);

    ///
    /// Add options to the interface.
    ///
    /// The options will be added to the default group.
    ///
    Interface& AddOptions(const std::vector<Option>& options);

    ///
    /// Add an option group to the interface.
    ///
    Interface& AddOptionGroup(const std::string& title, const std::vector<Option>& options);

    ///
    /// Add an option group to the interface.
    ///
    Interface& AddOptionGroup(const OptionGroup& group);

    ///
    /// Add an positional argument to the interface.
    ///
    /// Positional arguments will be handled in the order they are registered.
    ///
    Interface& AddPositionalArgument(const PositionalArgument& posArg);

    ///
    /// Add positional arguments to the interface.
    ///
    /// Positional arguments will be handled in the order they are registered.
    ///
    Interface& AddPositionalArguments(const std::vector<PositionalArgument>& posArgs);

    ///
    /// Set explicit default log level.
    ///
    /// The built-in log level defaults to WARN, if not set by user. Applications
    /// may use this method to override that default (e.g. to INFO) if desired.
    ///
    Interface& DefaultLogLevel(Logging::LogLevel level);

    ///
    /// Set application example.
    ///
    /// \note Currently only used when part of multi-tool interfaces.
    ///
    Interface& Example(std::string example);

    ///
    /// Set help display footer text.
    ///
    /// \note In order to preserve the precise layout of the footer text, it will
    ///       NOT be automatically word-wrapped.
    ///
    Interface& HelpFooter(std::string footer);

    ///
    /// Provide a custom application-defined logging config
    ///
    Interface& LogConfig(const Logging::LogConfig& config);

public:
    ///
    /// \return application's description
    ///
    const std::string& ApplicationDescription() const;

    ///
    /// \return application's name
    ///
    const std::string& ApplicationName() const;

    ///
    /// \return application's version
    ///
    const std::string& ApplicationVersion() const;

    ///
    /// \return current default log level (client application's may override)
    ///
    Logging::LogLevel DefaultLogLevel() const;

    ///
    /// \return application example
    ///
    /// \note Currently only used when part of multi-tool interfaces.
    ///
    const std::string& Example() const;

    ///
    /// \return true if any positional arguments are required
    ///
    bool HasRequiredPosArgs() const;

    ///
    /// \return help display footer text.
    ///
    const std::string& HelpFooter() const;

    ///
    /// \return logging configuration
    ///
    /// Default Logging::LogConfig if not otherwise specified.
    ///
    const Logging::LogConfig& LogConfig() const;

    /// \internal
    /// \return "flattened" list of all registered options (grouping is ignored)
    ///
    std::vector<internal::OptionData> Options() const;

    /// \internal
    /// \return all registered option groups
    ///
    const std::vector<internal::OptionGroupData>& OptionGroups() const;

    /// \internal
    /// \return all registered positional arguments
    ///
    const std::vector<internal::PositionalArgumentData>& PositionalArguments() const;

public:
    /// \internal
    /// \return (translated) help option.
    ///
    const internal::OptionData& HelpOption() const;

    /// \internal
    /// \return (translated) log file option.
    ///
    const internal::OptionData& LogFileOption() const;

    /// \internal
    /// \return (translated) log level option.
    ///
    const internal::OptionData& LogLevelOption() const;

    /// \internal
    /// \return (translated) numThreads option.
    const internal::OptionData& NumThreadsOption() const;

    /// \internal
    /// \return (translated) version option.
    ///
    const internal::OptionData& VersionOption() const;

public:
    ///
    /// Initializes the results object that will be populated from the command line
    /// or resolved tool contract
    ///
    Results MakeDefaultResults() const;

private:
    internal::InterfaceData data_;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACE_H
