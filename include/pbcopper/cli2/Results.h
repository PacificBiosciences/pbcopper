// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_RESULTS_H
#define PBCOPPER_CLI_v2_RESULTS_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/OptionValue.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/Result.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>
#include <pbcopper/logging/Logging.h>

namespace PacBio {
namespace CLI_v2 {

namespace internal {
class CommandLineParser;
}

///
/// Provides an application with values for all options & positional arguments
/// that were registered in its interface.
///
class Results
{
public:
    Results() = default;
    Results(const Results&) = delete;
    Results(Results&&) = default;
    Results& operator=(const Result&) = delete;
    Results& operator=(Results&&) = default;

public:
    ///
    /// \return full command line text
    ///
    const std::string& InputCommandLine() const;

    ///
    /// \return enum value for requested log level
    ///
    PacBio::Logging::LogLevel LogLevel() const;

    ///
    /// \return number of processors to use (e.g. from tool contract "nproc")
    ///
    size_t NumProcessors() const;

    ///
    /// \return full list of positional argument values
    ///
    const std::vector<std::string>& PositionalArguments() const;

    ///
    /// \return value associated with option
    ///
    /// The lookup uses the same Option as defined in the application. Example:
    ///
    ///   const Option X{...};
    ///   const Option S{...};
    ///
    ///   int x    = results[MyOptions::X];
    ///   string s = results[MyOptions::S];
    ///
    const Result& operator[](const Option& opt) const;

    ///
    /// \return value associated with positional argument
    ///
    /// The lookup uses the same PositionalArgument as defined in the
    /// application. Example:
    ///
    ///   const PositionalArgument In{...};
    ///   const PositionalArugment Out{...};
    ///
    ///   string in  = results[MyOptions::In];
    ///   string out = results[MyOptions::Out];
    ///
    const std::string& operator[](const PositionalArgument& posArg) const;

public:
    ///
    /// Registers an observed positional argument value
    ///
    Results& AddPositionalArgument(std::string arg);

    ///
    /// Initializes an option on this Results container, using its default value.
    ///
    Results& AddDefaultOption(const internal::OptionData& opt);

    ///
    /// Registers an observed flag turned on
    ///
    Results& AddObservedFlag(const std::string& name, const SetByMode setBy);

    ///
    /// Registers an observed option value
    ///
    Results& AddObservedValue(const std::string& name, OptionValue value, const SetByMode setBy);

    ///
    /// Sets the command line used to invoke the application
    ///
    Results& InputCommandLine(std::string cmdLine);

    ///
    /// Sets the requested log level
    ///
    Results& LogLevel(PacBio::Logging::LogLevel logLevel);

    ///
    /// Sets the requested number of processors to use (e.g. from "nproc")
    ///
    Results& NumProcessors(size_t nproc);

    ///
    /// Registers the interface's positional arguments.
    ///
    Results& PositionalArguments(const std::vector<internal::PositionalArgumentData>& posArgs);

private:
    std::string inputCommandLine_;

    PacBio::Logging::LogLevel logLevel_ = PacBio::Logging::LogLevel::INFO;
    size_t numProcessors_ = 1;

    std::unordered_map<std::string, std::shared_ptr<Result>> results_;
    std::vector<std::string> posArgNames_;
    std::vector<std::string> posArgValues_;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_RESULTS_H
