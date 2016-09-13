#include "pbcopper/cli/Interface.h"
#include "pbcopper/utility/EnumClassHash.h"
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
    typedef unordered_map<BuiltInOption,
                          bool,
                          Utility::EnumClassHash> BuiltInLookup;

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
    BuiltInLookup usingBuiltInOptions_;

    boost::optional<ToolContract::Config> tcConfig_;

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
    {
        if (appName_.empty())
            throw runtime_error("CLI::Interface: application name must not be empty");

        // initialize defaults - only turn on those requested
        usingBuiltInOptions_[BuiltInOption::Help]    = false;
        usingBuiltInOptions_[BuiltInOption::Version] = false;
        usingBuiltInOptions_[BuiltInOption::Verbose] = false;
        usingBuiltInOptions_[BuiltInOption::EmitToolContract]     = false;
        usingBuiltInOptions_[BuiltInOption::ResolvedToolContract] = false;
    }

    InterfacePrivate(const InterfacePrivate& other) = default;

public:
    void AddBuiltInOption(const BuiltInOption& type, const Option& option);
    void AddOption(const Option& option);
    void AddPositionalArgument(PositionalArg posArg);
};

void InterfacePrivate::AddBuiltInOption(const BuiltInOption& type,
                                        const Option& option)
{
    // register built-in
    auto found = usingBuiltInOptions_.find(type);
    if (found == usingBuiltInOptions_.end())
        throw std::runtime_error("CLI::Interface - unsupported built-in option requested");
    found->second = true;

    // add option
    AddOption(option);
}

void InterfacePrivate::AddOption(const Option& option)
{
    auto optionNames = option.Names();
    assert(!optionNames.empty());

    // ensure unique
    for (const auto& name : optionNames) {
        if (optionNameLookup_.find(name) != optionNameLookup_.cend())
            throw std::runtime_error("CLI::Interface: duplicate option name (" + name +")");
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

Interface& Interface::AddHelpOption(void)
{
    d_->AddBuiltInOption(BuiltInOption::Help,
                         Option{"help", {"h", "help"}, "Output this help."});
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

Interface& Interface::AddVerboseOption(void)
{
    d_->AddBuiltInOption(BuiltInOption::Verbose,
                         {"verbose", {"v", "verbose"}, "Use verbose output."});
    return *this;
}

Interface& Interface::AddVersionOption(void)
{
    d_->AddBuiltInOption(BuiltInOption::Version,
                         {"version", "version", "Output version info."});
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
    d_->AddBuiltInOption(BuiltInOption::EmitToolContract,
                         {"emit_tc", "emit-tool-contract", "Emit tool contract."});
    d_->AddBuiltInOption(BuiltInOption::ResolvedToolContract,
                         {"rtc_provided", "resolved-tool-contract", "Use args from resolved tool contract.", Option::StringType("")});

    d_->tcConfig_ = tcConfig;
    return *this;
}

bool Interface::ExpectsValue(const std::string& optionName) const
{
    const auto nameLookupIter = d_->optionNameLookup_.find(optionName);
    if (nameLookupIter == d_->optionNameLookup_.cend())
        throw std::runtime_error("unknown option name: "+optionName);
    const auto offset = nameLookupIter->second;
    const JSON::Json& defaultValue = d_->options_.at(offset).DefaultValue();
    return !defaultValue.is_boolean();
}

bool Interface::HasOptionRegistered(const std::string& optionName) const
{
    return d_->optionNameLookup_.find(optionName) !=
           d_->optionNameLookup_.cend();
}

std::string Interface::IdForOptionName(const std::string& optionName) const
{
    const auto nameLookupIter = d_->optionNameLookup_.find(optionName);
    if (nameLookupIter == d_->optionNameLookup_.cend())
        throw std::runtime_error("unknown option name: "+optionName);
    const auto offset = nameLookupIter->second;
    return d_->options_.at(offset).Id();
}

bool Interface::IsBuiltInOptionEnabled(const BuiltInOption& type) const
{
    const auto iter = d_->usingBuiltInOptions_.find(type);
    if (iter == d_->usingBuiltInOptions_.cend())
        throw std::runtime_error("CLI::Interface - unsupported built-in option requested");
    return iter->second;
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
