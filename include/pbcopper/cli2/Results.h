#ifndef PBCOPPER_CLI_v2_RESULTS_H
#define PBCOPPER_CLI_v2_RESULTS_H

#include <pbcopper/PbcopperConfig.h>

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

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
    Results(Results&&) noexcept = default;
    Results& operator=(const Result&) = delete;
    Results& operator=(Results&&) noexcept(std::is_nothrow_move_assignable<std::string>::value) =
        default;

public:
    ///
    /// \return the equivalent of InputCommandLine() plus all default values
    ///         that were not specified
    ///
    std::string EffectiveCommandLine() const;

    ///
    /// \return full command line text, verbatim, as provided
    ///
    const std::string& InputCommandLine() const;

    ///
    /// \return requested log file (empty string if not provided)
    ///
    std::string LogFile() const;

    ///
    /// \return enum value for requested log level
    ///
    PacBio::Logging::LogLevel LogLevel() const;

    ///
    /// \return number of threads to use, resolved from user-requested count and
    //          C++ implementation hints (see std::thread::hardware_concurrency)
    ///
    size_t NumThreads() const;

    ///
    /// \return requested alarms JSON file (empty string if not provided)
    ///
    std::string AlarmsFile() const;

    ///
    /// \return full list of positional argument values
    ///
    const std::vector<std::string>& PositionalArguments() const;

    ///
    /// \return true if verbosity is enabled & user-requested
    ///
    bool Verbose() const;

    ///
    /// \return true if --allow-exceptions-passthrough was passed for debugging
    ///
    bool ExceptionPassthrough() const;

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
    /// Registers the interface's positional arguments.
    ///
    Results& PositionalArguments(const std::vector<internal::PositionalArgumentData>& posArgs);

private:
    std::string inputCommandLine_;
    std::unordered_map<std::string, std::shared_ptr<Result>> results_;
    std::vector<std::string> posArgNames_;
    std::vector<std::string> posArgValues_;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_RESULTS_H
