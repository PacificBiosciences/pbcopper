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

#include <pbcopper/cli/Interface.h>

#include <cassert>
#include <unordered_map>

#include <boost/optional.hpp>

namespace PacBio {
namespace CLI {
namespace internal {

class InterfacePrivate
{
    typedef std::unordered_map<std::string, size_t>  NameLookup;

public:
    // application info
    std::string appName_;
    std::string appDescription_;
    std::string appVersion_;
    std::string alternativeToolContractName_;

    // parsing mode/status
    SingleDashMode singleDashMode_;
    bool isParsed_;
    std::string errorString_;

    // registered options
    std::vector<Option>        options_;
    std::vector<PositionalArg> positionalArgs_;
    NameLookup    optionNameLookup_;

    // option groups
    std::vector<std::string> optionGroupNames_;
    std::map<std::string, std::vector<size_t>> optionGroups_; // name -> options_[idx]

    // tool contract config
    boost::optional<ToolContract::Config> tcConfig_;

    // standard options
    boost::optional<Option> helpOption_;
    boost::optional<Option> logLevelOption_;
    boost::optional<Option> verboseOption_;
    boost::optional<Option> versionOption_;

public:
    InterfacePrivate(const std::string& appName,
                     const std::string& appDescription,
                     const std::string& appVersion)
        : appName_(appName)
        , appDescription_(appDescription)
        , appVersion_(appVersion)
        , singleDashMode_(SingleDashMode::ParseAsShortOptions)
        , isParsed_(false)
        , tcConfig_(boost::none)
        , helpOption_(boost::none)
        , logLevelOption_(boost::none)
        , verboseOption_(boost::none)
        , versionOption_(boost::none)
    {
        if (appName_.empty())
            throw std::runtime_error("CLI::Interface - application name must not be empty");
    }

    InterfacePrivate(const InterfacePrivate& other) = default;

public:
    void AddOption(const Option& option, const std::string& group = "Options");
    void AddPositionalArgument(PositionalArg posArg);
};

void InterfacePrivate::AddOption(const Option& option, const std::string& group)
{
    auto optionNames = option.Names();
    assert(!optionNames.empty());

    // ensure unique
    for (const auto& name : optionNames) {
        if (optionNameLookup_.find(name) != optionNameLookup_.cend())
            throw std::runtime_error("CLI::Interface - duplicate option name:"+name);
    }

    // store option
    options_.push_back(option);

    // store names in lookup
    const auto optionIndex = options_.size() - 1;
    for (const auto& name : optionNames)
        optionNameLookup_.insert(std::make_pair(name, optionIndex));

    // add option to group
    const auto nameIter = optionGroups_.find(group);
    if (nameIter == optionGroups_.cend())
        optionGroupNames_.push_back(group); // add new group
    optionGroups_[group].push_back(optionIndex);
}

void InterfacePrivate::AddPositionalArgument(PositionalArg posArg)
{
    if (posArg.syntax_.empty())
        posArg.syntax_ = posArg.name_;
    positionalArgs_.push_back(posArg);
}

} // namespace internal

// ------------------------
// PacBio::CLI::Interface
// ------------------------

Interface::Interface(const std::string& appName,
                     const std::string& appDescription,
                     const std::string& appVersion)
    : d_(new internal::InterfacePrivate{appName, appDescription, appVersion})
{ }

Interface::Interface(const Interface& other)
    : d_(new internal::InterfacePrivate{*other.d_.get()})
{ }

Interface::~Interface(void) { }

Interface& Interface::AddGroup(const std::string& name,
                               const std::vector<Option>& options)
{
    for (const auto& opt : options)
        d_->AddOption(opt, name);
    return *this;
}

Interface& Interface::AddHelpOption(const Option& option)
{
    d_->helpOption_ = option;
    d_->AddOption(option);
    return *this;
}

Interface& Interface::AddLogLevelOption(const Option& option)
{
    d_->logLevelOption_ = option;
    d_->AddOption(option);
    return *this;
}

Interface& Interface::AddOption(const Option& option)
{  return AddOptions( std::vector<Option>{ option }); }

Interface& Interface::AddOptions(const std::vector<Option>& options)
{
    for (const auto& opt : options)
        d_->AddOption(opt);
    return *this;
}

Interface& Interface::AddPositionalArgument(const PositionalArg& posArg)
{ return AddPositionalArguments(std::vector<PositionalArg>{ posArg }); }

Interface& Interface::AddPositionalArguments(const std::vector<PositionalArg>& posArgs)
{
    for (const auto& posArg : posArgs)
        d_->AddPositionalArgument(posArg);
    return *this;
}

Interface& Interface::AddVerboseOption(const Option& option)
{
    d_->verboseOption_ = option;
    d_->AddOption(option);
    return *this;
}

Interface& Interface::AddVersionOption(const Option& option)
{
    d_->versionOption_ = option;
    d_->AddOption(option);
    return *this;
}

std::string Interface::ApplicationDescription(void) const
{ return d_->appDescription_; }

Interface& Interface::ApplicationDescription(const std::string& description)
{ d_->appDescription_ = description; return *this; }

std::string Interface::ApplicationName(void) const
{ return d_->appName_; }

Interface& Interface::ApplicationName(const std::string& name)
{ d_->appName_ = name; return *this; }

std::string Interface::ApplicationVersion(void) const
{ return d_->appVersion_; }

Interface& Interface::ApplicationVersion(const std::string& version)
{ d_->appVersion_ = version; return *this; }

std::string Interface::AlternativeToolContractName(void) const
{ return d_->alternativeToolContractName_; }

Interface& Interface::AlternativeToolContractName(const std::string& version)
{ d_->alternativeToolContractName_ = version; return *this; }

Interface& Interface::EnableToolContract(const ToolContract::Config& tcConfig)
{
    d_->AddOption(Option{"emit_tc", "emit-tool-contract", "Emit tool contract."});
    d_->AddOption(Option{"rtc_provided", "resolved-tool-contract", "Use args from resolved tool contract.", Option::StringType("")});
    d_->tcConfig_ = tcConfig;
    return *this;
}

bool Interface::ExpectsValue(const std::string& optionName) const
{
    const auto nameLookupIter = d_->optionNameLookup_.find(optionName);
    if (nameLookupIter == d_->optionNameLookup_.cend())
        throw std::runtime_error("CLI::Interface - unknown option name: "+optionName);
    const auto offset = nameLookupIter->second;
    const JSON::Json& defaultValue = d_->options_.at(offset).DefaultValue();
    return !defaultValue.is_boolean();
}

std::vector<std::string> Interface::Groups(void) const
{ return d_->optionGroupNames_; }

std::vector<Option> Interface::GroupOptions(const std::string& group) const
{
    std::vector<Option> result;
    const auto indices = d_->optionGroups_.at(group);
    result.reserve(indices.size());
    for (const auto& idx : indices)
        result.push_back(d_->options_.at(idx));
    return result;
}

bool Interface::HasHelpOptionRegistered(void) const
{ return d_->helpOption_.is_initialized(); }

bool Interface::HasLogLevelOptionRegistered(void) const
{ return d_->logLevelOption_.is_initialized(); }

bool Interface::HasVerboseOptionRegistered(void) const
{ return d_->verboseOption_.is_initialized(); }

bool Interface::HasVersionOptionRegistered(void) const
{ return d_->versionOption_.is_initialized(); }

bool Interface::HasOptionRegistered(const std::string& optionName) const
{
    return d_->optionNameLookup_.find(optionName) !=
           d_->optionNameLookup_.cend();
}

const Option& Interface::HelpOption(void) const
{
    if (!HasHelpOptionRegistered())
        throw std::runtime_error("CLI::Interface - help option requested, but not registered");
    return d_->helpOption_.get();
}

std::string Interface::IdForOptionName(const std::string& optionName) const
{
    const auto nameLookupIter = d_->optionNameLookup_.find(optionName);
    if (nameLookupIter == d_->optionNameLookup_.cend())
        throw std::runtime_error("CLI::Interface - unknown option name: "+optionName);
    const auto offset = nameLookupIter->second;
    return d_->options_.at(offset).Id();
}

bool Interface::IsToolContractEnabled(void) const
{ return d_->tcConfig_.is_initialized(); }

const Option& Interface::LogLevelOption(void) const
{
    if (!HasLogLevelOptionRegistered())
        throw std::runtime_error("CLI::Interface - log level option requested, but not registered");
    return d_->logLevelOption_.get();
}

PacBio::JSON::Json Interface::OptionChoices(const std::string& optionId) const
{
    for (const auto& opt : d_->options_) {
        if (opt.Id() == optionId)
            return opt.Choices();
    }
    return PacBio::JSON::Json(nullptr);
}

std::vector<Option> Interface::RegisteredOptions(void) const
{ return d_->options_; }

std::vector<PositionalArg> Interface::RegisteredPositionalArgs(void) const
{ return d_->positionalArgs_; }

const ToolContract::Config& Interface::ToolContract(void) const
{
    if (!d_->tcConfig_)
        throw std::runtime_error("CLI::Interface - requesting tool contract config for an interface that has not enabled this feature.");
    return d_->tcConfig_.get();
}

const Option& Interface::VerboseOption(void) const
{
    if (!HasVerboseOptionRegistered())
        throw std::runtime_error("CLI::Interface - verbose option requested, but not registered");
    return d_->verboseOption_.get();
}

const Option& Interface::VersionOption(void) const
{
    if (!HasVersionOptionRegistered())
        throw std::runtime_error("CLI::Interface - version option requested, but not registered");
    return d_->versionOption_.get();
}

} // namespace CLI
} // namespace PacBio
