#include "pbcopper/cli/Results.h"

#include "pbcopper/json/JSON.h"

#include <unordered_map>
#include <set>
#include <vector>
#include <cassert>
#include <cstdlib>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::JSON;

using namespace std;

namespace PacBio {
namespace CLI {
namespace internal {

class ResultsPrivate
{
public:
    PacBio::CLI::Interface interface_;
    Json options_;
    vector<string> positionalArgs_;

public:
    ResultsPrivate(const Interface& interface)
        : interface_(interface)
    {
        // init with default values
        const auto& registeredOptions = interface_.RegisteredOptions();
        for (const Option& opt : registeredOptions)
            options_[opt.Id()] = opt.DefaultValue();
    }

    ResultsPrivate(const ResultsPrivate& other) = default;
    ~ResultsPrivate(void)  = default;
};

} // namespace internal
} // namespace CLI
} // namespace PacBio

// ------------------------
// PacBio::CLI::Results
// ------------------------

Results::Results(const Interface& interface)
    : d_(new internal::ResultsPrivate{ interface })
{ }

Results::Results(const Results& other)
    : d_(new internal::ResultsPrivate{*other.d_.get()})
{ }

Results::~Results(void) { }

Results& Results::RegisterOptionValue(const string& optionId,
                                      const Json& optionValue)
{
    // TODO: make safe access
    d_->options_[optionId] = optionValue;
    return *this;
}

Results& Results::RegisterOptionValueString(const string& optionId,
                                            const string& optionValue)
{
    // TODO: make safe access
    Json& opt = d_->options_[optionId];
    switch(opt.type())
    {
        case Json::value_t::number_integer  : opt = std::strtol(optionValue.c_str(), nullptr, 10);  break;
        case Json::value_t::number_unsigned : opt = std::strtoul(optionValue.c_str(), nullptr, 10); break;
        case Json::value_t::number_float    : opt = std::strtof(optionValue.c_str(), nullptr);  break;
        case Json::value_t::string  : opt = optionValue; break;
        case Json::value_t::boolean : opt = (optionValue == "true"); break;
        default:
            throw std::runtime_error("PacBio::CLI::Results - unknown type for option: "+optionId);
    }

    return *this;
}

Results& Results::RegisterPositionalArg(const string& posArg)
{ d_->positionalArgs_.push_back(posArg); return *this; }

const Interface& Results::ApplicationInterface(void) const
{ return d_->interface_; }

//std::string Results::PositionalArgument(const std::string& posArgName) const
//{
//    const auto& registeredPosArgs = d_->interface_.RegisteredPositionalArgs();
//    size_t index = 0;
//    for (const PositionalArg& registeredArg : registeredPosArgs) {
//        if (posArgName == registeredArg.name_)
//            return d_->positionalArgs_.at(index);
//        ++index;
//    }
//    throw std::runtime_error("PacBio::CLI::Results - unknown positional arg name: "+posArgName);
//}

vector<string> Results::PositionalArguments(void) const
{ return d_->positionalArgs_; }

Results& Results::RegisterObservedOption(const std::string& optionId)
{
    // TODO: make safe access
    Json& opt = d_->options_[optionId];
    if (opt.is_boolean())
        opt = true;
    return *this;
}

const Results::Result& Results::operator[](const std::string& optionId) const
{
    // TODO: make safe access
    return d_->options_[optionId];
}

Results::Result& Results::operator[](const std::string& optionId)
{
    // TODO: make safe access
    return d_->options_[optionId];
}
