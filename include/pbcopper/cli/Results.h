// Author: Derek Barnett

#ifndef PBCOPPER_CLI_RESULTS_H
#define PBCOPPER_CLI_RESULTS_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/Interface.h>
#include <pbcopper/cli/Option.h>
#include <pbcopper/json/JSON.h>
#include <pbcopper/logging/Logging.h>

namespace PacBio {
namespace CLI {

///
/// \brief The Results class
///
class Results
{
public:
    using Result = JSON::Json;

public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief Results
    /// \param interface
    ///
    Results(Interface interface);

    Results(Interface interface, std::vector<std::string> inputCommandLine);

    Results(const Results& other);
    Results(Results&& other) noexcept;
    Results& operator=(const Results& other);
    Results& operator=(Results&& other) noexcept;
    ~Results();

    /// \}

public:
    /// \name CLI Results Query
    /// \{

    ///
    /// \brief ApplicationInterface
    /// \return
    ///
    const Interface& ApplicationInterface() const;

    ///
    /// \brief InputCommandLine
    /// \return
    ///
    std::string InputCommandLine() const;

    ///
    /// \bbrief EffectiveCommandLine
    /// \return vector of command-line args to repeat this call, but with explicit defaults
    ///
    std::string EffectiveCommandLine() const;

    ///
    /// \brief IsFromRTC
    /// \return true if this Results object generated from a resolved tool
    ///         contract (as opposed to command-line args)
    ///
    bool IsFromRTC() const;

    ///
    /// \brief LogLevel
    ///
    /// Default level
    ///
    /// \note Currently only set by resolved tool contract. Setting from
    ///       command-line will follow shortly.
    ///
    /// \return
    ///
    PacBio::Logging::LogLevel LogLevel() const;

    ///
    /// \brief NumProcessors
    ///
    /// \note Currently, this field is \b only populated when the Results
    ///       object was created from a resolved tool contract.
    ///
    /// \sa IsFromRTC to check if this will be available
    ///
    /// \return number of processors specified by resolved tool contract.
    ///
    uint16_t NumProcessors() const;

    ///
    /// \brief PositionalArguments
    /// \return
    ///
    std::vector<std::string> PositionalArguments() const;

    //    std::string PositionalArgument(const std::string& posArgName) const;

    ///
    /// \brief operator []
    /// \param optionId
    /// \return
    ///
    const Result& operator[](const std::string& optionId) const;

    ///
    /// \brief operator []
    /// \param id
    /// \return
    ///
    Result& operator[](const std::string& id);

    /// \}

public:
    /// \name Results Construction
    /// \{

    ///
    /// \brief LogLevel
    ///
    /// \note Intended for internal use only. This method will likely disappear
    ///       in a future version.
    ///
    /// \param logLevel
    /// \return
    ///
    Results& LogLevel(const PacBio::Logging::LogLevel logLevel);

    ///
    /// \brief NumProcessors
    ///
    /// \note Intended for internal use only. This method will likely disappear
    ///       in a future version.
    ///
    /// \param nproc
    /// \return
    ///
    Results& NumProcessors(const uint16_t nproc);

    ///
    /// \brief Registers observed positional arg.
    ///
    /// Used in constructing results from command line or RTC; unlikely needed
    /// by client code.
    ///
    /// \param arg
    /// \return
    ///
    Results& RegisterPositionalArg(const std::string& arg);

    ///
    /// \brief RegisterObservedOption
    ///
    /// Used in constructing results from command line or RTC; unlikely needed
    /// by client code.
    ///
    /// \param optionId
    /// \return
    ///
    Results& RegisterObservedOption(const std::string& optionId);

    ///
    /// \brief RegisterOptionValue
    ///
    /// Used in constructing results from command line or RTC; unlikely needed
    /// by client code.
    ///
    /// \param optionId
    /// \param optionValue
    /// \return
    ///
    Results& RegisterOptionValue(const std::string& optionId, const JSON::Json& optionValue);

    ///
    /// \brief RegisterOptionValueString
    ///
    /// Used in constructing results from command line or RTC; unlikely needed
    /// by client code.
    ///
    /// \param optionId
    /// \param optionValue
    /// \return
    ///
    Results& RegisterOptionValueString(const std::string& optionId, const std::string& optionValue);

    ///
    /// \brief SetFromRTC
    ///
    /// \note Intended for internal use only. This method will likely disappear
    ///       in a future version.
    ///
    /// \param ok
    /// \return
    ///
    Results& SetFromRTC(const bool ok = true);

    /// \}

private:
    class ResultsPrivate;
    std::unique_ptr<ResultsPrivate> d_;
};

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_RESULTS_H
