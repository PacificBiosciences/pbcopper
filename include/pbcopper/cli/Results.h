#ifndef PBCOPPER_CLI_RESULTS_H
#define PBCOPPER_CLI_RESULTS_H

#include "pbcopper/Config.h"
#include "pbcopper/cli/Interface.h"
#include "pbcopper/cli/Option.h"
#include "pbcopper/logging/Logging.h"
#include "pbcopper/json/JSON.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace PacBio {
namespace CLI {

namespace internal { class ResultsPrivate; }

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
    Results(const Interface& interface);

    Results(const Interface& interface,
            const std::vector<std::string>& inputCommandLine);

    Results(const Results& other);
    Results& operator=(const Results& other);

    ~Results(void);

    /// \}

public:
    /// \name CLI Results Query
    /// \{

    ///
    /// \brief ApplicationInterface
    /// \return
    ///
    const Interface& ApplicationInterface(void) const;

    ///
    /// \brief InputCommandLine
    /// \return
    ///
    std::string InputCommandLine(void) const;

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
    PacBio::Logging::LogLevel LogLevel(void) const;

    ///
    /// \brief PositionalArguments
    /// \return
    ///
    std::vector<std::string> PositionalArguments(void) const;

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

    Results& LogLevel(const PacBio::Logging::LogLevel logLevel);

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
    Results& RegisterOptionValue(const std::string& optionId,
                                 const JSON::Json& optionValue);

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
    Results& RegisterOptionValueString(const std::string& optionId,
                                       const std::string& optionValue);

    /// \}

private:
    std::unique_ptr<internal::ResultsPrivate> d_;
};

} // namespace CLI
} // namespace PacBio

#include "internal/Results-inl.h"

#endif // PBCOPPER_CLI_RESULTS_H
