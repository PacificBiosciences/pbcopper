// Author: Derek Barnett

#include <pbcopper/cli/Results.h>

#include <cassert>
#include <cstdlib>
#include <set>
#include <unordered_map>
#include <vector>

#include <pbcopper/json/JSON.h>
#include <pbcopper/utility/StringUtils.h>

#include <sstream>
using Json = PacBio::JSON::Json;

namespace PacBio {
namespace CLI {

class Results::ResultsPrivate
{
public:
    PacBio::CLI::Interface interface_;
    std::vector<std::string> inputCommandLine_;
    Json options_;
    std::vector<std::string> positionalArgs_;
    PacBio::Logging::LogLevel logLevel_;
    bool isFromRtc_;
    uint16_t nproc_;

    ResultsPrivate(Interface interface, std::vector<std::string> inputCommandLine)
        : interface_{std::move(interface)}
        , inputCommandLine_{std::move(inputCommandLine)}
        , logLevel_{PacBio::Logging::LogLevel::INFO}
        , isFromRtc_{false}
        , nproc_{1}
    {
        // init with default values
        const auto registeredOptions = interface_.RegisteredOptions();
        for (const Option& opt : registeredOptions) {
            options_[opt.Id()] = opt.DefaultValue();
        }
    }
};

// ------------------------
// PacBio::CLI::Results
// ------------------------

Results::Results(Interface interface)
    : d_{std::make_unique<ResultsPrivate>(std::move(interface), std::vector<std::string>())}
{
}

Results::Results(Interface interface, std::vector<std::string> inputCommandLine)
    : d_(std::make_unique<ResultsPrivate>(std::move(interface), std::move(inputCommandLine)))
{
}

Results::Results(const Results& other) : d_{std::make_unique<ResultsPrivate>(*other.d_.get())} {}

Results::Results(Results&& other) noexcept = default;

Results& Results::operator=(const Results& other)
{
    if (this != &other) *this = Results{other};
    return *this;
}

Results& Results::operator=(Results&& other) noexcept = default;

Results::~Results() = default;

std::string Results::InputCommandLine() const { return Utility::Join(d_->inputCommandLine_, " "); }

std::string Results::EffectiveCommandLine() const
{
    std::ostringstream sout;
    const std::vector<Option> registeredOptions = d_->interface_.RegisteredOptions();
    for (const Option& opt : registeredOptions) {
        const auto defVal = opt.DefaultValue();
        if (defVal.is_boolean()) {
            const std::string alias = opt.Names().front();  // The last is probably the short one.
            // Print only if not false.
            if (d_->options_[opt.Id()]) {
                if (1 == alias.size()) {
                    // typical boolean flag
                    sout << " -" << alias;
                } else {
                    // just a guess
                    sout << " --" << alias;
                }
            }
        } else {
            const std::string alias =
                opt.Names().back();  // The last is probably the most descriptive.
            if (1 == alias.size()) {
                // just a guess
                sout << " -" << alias << " " << d_->options_[opt.Id()];
            } else {
                // typical
                sout << " --" << alias << "=" << d_->options_[opt.Id()];
            }
        }
    }
    for (const auto arg : d_->positionalArgs_) {
        sout << " " << arg;
    }
    return sout.str();
}

bool Results::IsFromRTC() const { return d_->isFromRtc_; }

Results& Results::LogLevel(const PacBio::Logging::LogLevel logLevel)
{
    d_->logLevel_ = logLevel;
    return *this;
}

PacBio::Logging::LogLevel Results::LogLevel() const { return d_->logLevel_; }

uint16_t Results::NumProcessors() const
{
    if (!IsFromRTC()) {
        throw std::runtime_error(
            "PacBio::CLI::Results - NumProcessors() "
            "available from resolved tool contract only. "
            "Use IsFromRTC() to check before calling this "
            "method.");
    }
    return d_->nproc_;
}

Results& Results::NumProcessors(const uint16_t nproc)
{
    d_->nproc_ = nproc;
    return *this;
}

Results& Results::RegisterOptionValue(const std::string& optionId, const Json& optionValue)
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
    const auto choices = d_->interface_.OptionChoices(optionId);
    if (choices.is_array() && !choices.empty()) {
        bool match = false;
        for (const auto& e : choices) {
            if (e == optionValue) {
                match = true;
                break;
            }
        }
        if (!match) {
            std::string msg = std::string{"PacBio::CLI::Results - encountered unsupported value: "};
            msg += optionValue.dump() + std::string{" for "} + optionId;
            msg += std::string{"\n  Valid choices are: "} + choices.dump();
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
    switch (opt.type()) {
        case Json::value_t::number_integer:
            opt = std::strtol(optionValue.c_str(), nullptr, 10);
            break;
        case Json::value_t::number_unsigned:
            opt = std::strtoul(optionValue.c_str(), nullptr, 10);
            break;
        case Json::value_t::number_float:
            opt = std::strtof(optionValue.c_str(), nullptr);
            break;
        case Json::value_t::string:
            opt = optionValue;
            break;
        case Json::value_t::boolean:
            opt = (optionValue == "true");
            break;
        default:
            throw std::runtime_error("PacBio::CLI::Results - unknown type for option: " + optionId);
    }

    return RegisterOptionValue(optionId, opt);
}

Results& Results::RegisterPositionalArg(const std::string& posArg)
{
    d_->positionalArgs_.push_back(posArg);
    return *this;
}

Results& Results::SetFromRTC(const bool ok)
{
    d_->isFromRtc_ = ok;
    return *this;
}

const Interface& Results::ApplicationInterface() const { return d_->interface_; }

std::vector<std::string> Results::PositionalArguments() const { return d_->positionalArgs_; }

Results& Results::RegisterObservedOption(const std::string& optionId)
{
    // TODO: make safe access
    Json& opt = d_->options_[optionId];
    if (opt.is_boolean()) opt = true;
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

}  // namespace CLI
}  // namespace PacBio
