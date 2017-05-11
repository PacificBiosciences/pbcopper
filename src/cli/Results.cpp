#include "pbcopper/cli/Results.h"
#include "pbcopper/json/JSON.h"
#include "pbcopper/utility/StringUtils.h"
#include <unordered_map>
#include <set>
#include <vector>
#include <cassert>
#include <cstdlib>

using Json = PacBio::JSON::Json;

namespace PacBio {
namespace CLI {
namespace internal {

class ResultsPrivate
{
public:
    PacBio::CLI::Interface interface_;
    std::vector<std::string> inputCommandLine_;
    Json options_;
    std::vector<std::string> positionalArgs_;
    PacBio::Logging::LogLevel logLevel_;
    bool isFromRtc_;
    uint16_t nproc_;

public:
    ResultsPrivate(const Interface& interface,
                   const std::vector<std::string>& inputCommandLine)
        : interface_(interface)
        , inputCommandLine_(inputCommandLine)
        , logLevel_(PacBio::Logging::LogLevel::INFO)
        , isFromRtc_(false)
        , nproc_(1)
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

// ------------------------
// PacBio::CLI::Results
// ------------------------

Results::Results(const Interface& interface)
    : d_(new internal::ResultsPrivate{ interface, std::vector<std::string>() })
{ }

Results::Results(const Interface& interface,
                 const std::vector<std::string>& inputCommandLine)
    : d_(new internal::ResultsPrivate{ interface, inputCommandLine })
{ }

Results::Results(const Results& other)
    : d_(new internal::ResultsPrivate{*other.d_.get()})
{ }

Results::Results(Results&& other)
    : d_(std::move(other.d_))
{ }

Results& Results::operator=(const Results& other)
{
    if (this != &other) {
        d_.reset(new internal::ResultsPrivate{*other.d_.get()});
    }
    return *this;
}

Results& Results::operator=(Results&& other)
{
    if (this != &other) {
        d_ = std::move(other.d_);
    }
    return *this;
}

Results::~Results(void) { }

std::string Results::InputCommandLine(void) const
{
    return Utility::Join(d_->inputCommandLine_, " ");
}

bool Results::IsFromRTC(void) const
{ return d_->isFromRtc_; }

Results& Results::LogLevel(const PacBio::Logging::LogLevel logLevel)
{
    d_->logLevel_ = logLevel;
    return *this;
}

PacBio::Logging::LogLevel Results::LogLevel(void) const
{ return d_->logLevel_; }

uint16_t Results::NumProcessors(void) const
{
    if (!IsFromRTC()) {
        throw std::runtime_error("PacBio::CLI::Results - NumProcessors() "
                                 "available from resolved tool contract only. "
                                 "Use IsFromRTC() to check before calling this "
                                 "method.");
    }
    return d_->nproc_;
}

Results& Results::NumProcessors(const uint16_t nproc)
{ d_->nproc_ = nproc; return *this; }

Results& Results::RegisterOptionValue(const std::string& optionId,
                                      const Json& optionValue)
{
    // TODO: make safe access
    d_->options_[optionId] = optionValue;

    // check user-supplied log level
    if (d_->interface_.HasLogLevelOptionRegistered()) {
        const auto logLevelOptionId = d_->interface_.LogLevelOption().Id();
        if (logLevelOptionId == optionId) {
            const std::string logLevelString = optionValue;
            d_->logLevel_ = PacBio::Logging::LogLevel(logLevelString);
        }
    }

    // check choices
    const auto& choices = d_->interface_.OptionChoices(optionId);
    if (choices.is_array() && !choices.empty()) {
        bool match = false;
        for (const auto& e : choices) {
            if (e == optionValue) {
                match = true;
                break;
            }
        }
        if (!match) {
            std::string msg = std::string{ "PacBio::CLI::Results - encountered unsupported value: " };
            msg += optionValue.dump() + std::string{ " for " } + optionId;
            msg += std::string{ "\n  Valid choices are: " } + choices.dump();
            throw std::runtime_error(msg);
        }
    }

    return *this;
}

Results& Results::RegisterOptionValueString(const std::string& optionId,
                                            const std::string& optionValue)
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

    return RegisterOptionValue(optionId, opt);
}

Results& Results::RegisterPositionalArg(const std::string& posArg)
{ d_->positionalArgs_.push_back(posArg); return *this; }

Results& Results::SetFromRTC(const bool ok)
{ d_->isFromRtc_ = ok; return *this; }

const Interface& Results::ApplicationInterface(void) const
{ return d_->interface_; }

std::vector<std::string> Results::PositionalArguments(void) const
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

} // namespace CLI
} // namespace PacBio
