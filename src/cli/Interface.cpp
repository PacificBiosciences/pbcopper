#include "pbcopper/cli/Interface.h"
#include <boost/optional.hpp>
#include <unordered_map>
#include <cassert>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

namespace PacBio {
namespace CLI {
namespace internal {

class InterfacePrivate
{
    typedef unordered_map<string, size_t>  NameLookup;

public:
    // application info
    string appName_;
    string appDescription_;
    string appVersion_;
    string alternativeToolContractName_;

    // parsing mode/status
    SingleDashMode singleDashMode_;
    bool isParsed_;
    string errorString_;

    // registered options
    vector<Option>        options_;
    vector<PositionalArg> positionalArgs_;
    NameLookup    optionNameLookup_;

    boost::optional<ToolContract::Config> tcConfig_;

    boost::optional<Option> helpOption_;
    boost::optional<Option> logLevelOption_;
    boost::optional<Option> verboseOption_;
    boost::optional<Option> versionOption_;

public:
    InterfacePrivate(const string& appName,
                     const string& appDescription,
                     const string& appVersion)
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
            throw runtime_error("CLI::Interface - application name must not be empty");
    }

    InterfacePrivate(const InterfacePrivate& other) = default;

public:
    void AddOption(const Option& option);
    void AddPositionalArgument(PositionalArg posArg);
};

void InterfacePrivate::AddOption(const Option& option)
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
}

void InterfacePrivate::AddPositionalArgument(PositionalArg posArg)
{
    if (posArg.syntax_.empty())
        posArg.syntax_ = posArg.name_;
    positionalArgs_.push_back(posArg);
}

} // namespace internal
} // namespace CLI
} // namespace PacBio

// ------------------------
// PacBio::CLI::Interface
// ------------------------

Interface::Interface(const string& appName,
                     const string& appDescription,
                     const string& appVersion)
    : d_(new internal::InterfacePrivate{appName, appDescription, appVersion})
{ }

Interface::Interface(const Interface& other)
    : d_(new internal::InterfacePrivate{*other.d_.get()})
{ }

Interface::~Interface(void) { }

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
{  return AddOptions( vector<Option>{ option }); }

Interface& Interface::AddOptions(const vector<Option>& options)
{
    for (const auto& opt : options)
        d_->AddOption(opt);
    return *this;
}

Interface& Interface::AddPositionalArgument(const PositionalArg& posArg)
{ return AddPositionalArguments(vector<PositionalArg>{ posArg }); }

Interface& Interface::AddPositionalArguments(const vector<PositionalArg>& posArgs)
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

string Interface::ApplicationDescription(void) const
{ return d_->appDescription_; }

Interface& Interface::ApplicationDescription(const string& description)
{ d_->appDescription_ = description; return *this; }

string Interface::ApplicationName(void) const
{ return d_->appName_; }

Interface& Interface::ApplicationName(const string& name)
{ d_->appName_ = name; return *this; }

string Interface::ApplicationVersion(void) const
{ return d_->appVersion_; }

Interface& Interface::ApplicationVersion(const string& version)
{ d_->appVersion_ = version; return *this; }

string Interface::AlternativeToolContractName(void) const
{ return d_->alternativeToolContractName_; }

Interface& Interface::AlternativeToolContractName(const string& version)
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

vector<Option> Interface::RegisteredOptions(void) const
{ return d_->options_; }

vector<PositionalArg> Interface::RegisteredPositionalArgs(void) const
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
