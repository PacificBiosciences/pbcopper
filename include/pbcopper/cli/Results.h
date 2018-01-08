// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

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
#include <pbcopper/logging/Logging.h>
#include <pbcopper/json/JSON.h>

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
    Results(Results&& other);
    Results& operator=(const Results& other);
    Results& operator=(Results&& other);
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
    /// \brief IsFromRTC
    /// \return true if this Results object generated from a resolved tool
    ///         contract (as opposed to command-line args)
    ///
    bool IsFromRTC(void) const;

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
    /// \brief NumProcessors
    ///
    /// \note Currently, this field is \b only populated when the Results
    ///       object was created from a resolved tool contract.
    ///
    /// \sa IsFromRTC to check if this will be available
    ///
    /// \return number of processors specified by resolved tool contract.
    ///
    uint16_t NumProcessors(void) const;

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
    std::unique_ptr<internal::ResultsPrivate> d_;
};

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_RESULTS_H
