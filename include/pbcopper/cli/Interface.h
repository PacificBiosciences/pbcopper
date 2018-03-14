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

#ifndef PBCOPPER_CLI_INTERFACE_H
#define PBCOPPER_CLI_INTERFACE_H

#include <memory>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/Option.h>
#include <pbcopper/cli/PositionalArg.h>
#include <pbcopper/cli/SingleDashMode.h>
#include <pbcopper/cli/toolcontract/Config.h>

namespace PacBio {
namespace CLI {

namespace internal {
class InterfacePrivate;
}

///
/// \brief The Interface class
///
class Interface
{
public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief Interface
    /// \param appName
    /// \param appDescription
    /// \param appVersion
    ///
    Interface(const std::string& appName, const std::string& appDescription = std::string(),
              const std::string& appVersion = std::string());

    ///
    /// \brief Interface
    /// \param other
    ///
    Interface(const Interface& other);

    ~Interface(void);

    /// \}

public:
    /// \name Application Attributes
    /// \{

    ///
    /// \brief ApplicationDescription
    /// \return
    ///
    std::string ApplicationDescription(void) const;

    ///
    /// \brief ApplicationName
    /// \return
    ///
    std::string ApplicationName(void) const;

    ///
    /// \brief ApplicationVersion
    /// \return
    ///
    std::string ApplicationVersion(void) const;

    ///
    /// \brief AlternativeToolContractName
    /// \return
    ///
    std::string AlternativeToolContractName(void) const;

    /// \}

public:
    /// \name Interface Query
    /// \{

    ///
    /// \brief ExpectsValue
    /// \param optionName
    /// \return
    ///
    bool ExpectsValue(const std::string& optionName) const;

    ///
    /// \brief Groups
    /// \return
    ///
    std::vector<std::string> Groups(void) const;

    ///
    /// \brief GroupOptions
    /// \param name
    /// \return
    ///
    std::vector<Option> GroupOptions(const std::string& group) const;

    ///
    /// \brief HasOptionRegistered
    /// \param optionName
    /// \return
    ///
    bool HasOptionRegistered(const std::string& optionName) const;

    ///
    /// \brief HasHelpOptionRegistered
    /// \return
    ///
    bool HasHelpOptionRegistered(void) const;

    ///
    /// \brief HasLogLevelOptionRegistered
    /// \return
    ///
    bool HasLogLevelOptionRegistered(void) const;

    ///
    /// \brief HasVerboseOptionRegistered
    /// \return
    ///
    bool HasVerboseOptionRegistered(void) const;

    ///
    /// \brief HasVersionOptionRegistered
    /// \return
    ///
    bool HasVersionOptionRegistered(void) const;

    ///
    /// \brief HelpOption
    /// \return
    ///
    const Option& HelpOption(void) const;

    ///
    /// \brief IdForOptionName
    /// \param optionName
    /// \return
    ///
    std::string IdForOptionName(const std::string& optionName) const;

    ///
    /// \brief IsToolContractEnabled
    /// \return
    ///
    bool IsToolContractEnabled(void) const;

    ///
    /// \brief LogLevelOption
    /// \return
    ///
    const Option& LogLevelOption(void) const;

    ///
    /// \brief OptionChoices
    /// \param optionId
    /// \return
    ///
    PacBio::JSON::Json OptionChoices(const std::string& optionId) const;

    ///
    /// \brief RegisteredOptions
    /// \return
    ///
    std::vector<Option> RegisteredOptions(void) const;

    ///
    /// \brief RegisteredPositionalArgs
    /// \return
    ///
    std::vector<PositionalArg> RegisteredPositionalArgs(void) const;

    ///
    /// \brief ToolContract
    /// \return
    ///
    const ToolContract::Config& ToolContract(void) const;

    ///
    /// \brief VerboseOption
    /// \return
    ///
    const Option& VerboseOption(void) const;

    ///
    /// \brief VersionOption
    /// \return
    ///
    const Option& VersionOption(void) const;

    /// \}

public:
    /// \name Application Attributes
    /// \{

    ///
    /// \brief ApplicationDescription
    /// \param description
    /// \return
    ///
    Interface& ApplicationDescription(const std::string& description);

    ///
    /// \brief ApplicationName
    /// \param name
    /// \return
    ///
    Interface& ApplicationName(const std::string& name);

    ///
    /// \brief ApplicationVersion
    /// \param version
    /// \return
    ///
    Interface& ApplicationVersion(const std::string& version);

    ///
    /// \brief AlternativeToolContractName
    /// This name, if set, will be used as the alternative prefix of the
    /// tool contract option prefix and replace the actual ApplicationName.
    ///
    /// \param name
    /// \return
    ///
    Interface& AlternativeToolContractName(const std::string& name);

    /// \}

public:
    /// \name Interface Setup
    /// \{

    ///
    /// \brief AddGroup
    /// \param name
    /// \param option
    /// \return
    ///
    Interface& AddGroup(const std::string& name, const std::vector<Option>& option);

    ///
    /// \brief AddOption
    /// \param option
    /// \return
    ///
    Interface& AddOption(const Option& option);

    ///
    /// \brief AddOptions
    /// \param options
    /// \return
    ///
    Interface& AddOptions(const std::vector<Option>& options);

    ///
    /// \brief AddPositionalArgument
    /// \param posArg
    /// \return
    ///
    Interface& AddPositionalArgument(const PositionalArg& posArg);

    ///
    /// \brief AddPositionalArguments
    /// \param posArgs
    /// \return
    ///
    Interface& AddPositionalArguments(const std::vector<PositionalArg>& posArgs);

    ///
    /// \brief AddHelpOption
    /// \param option
    /// \return
    ///
    Interface& AddHelpOption(const Option& option = Option::DefaultHelpOption());

    ///
    /// \brief AddLogLevelOption
    /// \param option
    /// \return
    ///
    Interface& AddLogLevelOption(const Option& option = Option::DefaultLogLevelOption());

    ///
    /// \brief AddVerboseOption
    /// \param option
    /// \return
    ///
    Interface& AddVerboseOption(const Option& option = Option::DefaultVerboseOption());

    ///
    /// \brief AddVersionOption
    /// \param option
    /// \return
    ///
    Interface& AddVersionOption(const Option& option = Option::DefaultVersionOption());

    ///
    /// \brief EnableToolContract
    /// \param tcConfig
    /// \return
    ///
    Interface& EnableToolContract(const ToolContract::Config& tcConfig);

    /// \}

private:
    std::unique_ptr<internal::InterfacePrivate> d_;
};

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_INTERFACE_H
