#include <pbcopper/cli2/Results.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>
#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace CLI_v2 {

Results& Results::AddPositionalArgument(std::string arg)
{
    posArgValues_.emplace_back(std::move(arg));
    return *this;
}

Results& Results::AddDefaultOption(const internal::OptionData& opt)
{
    // make default value
    auto result = std::make_shared<Result>(opt.defaultValue.get(), SetByMode::DEFAULT);

    // add for all option names
    for (const auto& name : opt.names)
        results_.insert({name, result});
    for (const auto& hiddenName : opt.hiddenNames)
        results_.insert({hiddenName, result});

    return *this;
}

Results& Results::AddObservedFlag(const std::string& name, const SetByMode setBy)
{
    AddObservedValue(name, OptionValue{true}, setBy);
    return *this;
}

Results& Results::AddObservedValue(const std::string& name, OptionValue value,
                                   const SetByMode setBy)
{
    const auto found = results_.find(name);
    if (found == results_.cend()) {
        auto result = std::make_shared<Result>(value, setBy);
        results_.insert({name, std::move(result)});
    } else
        *found->second = Result{value, setBy};

    return *this;
}

std::string Results::EffectiveCommandLine() const
{
    std::ostringstream out;

    // application name
    const auto tokens = PacBio::Utility::Split(inputCommandLine_, ' ');
    const auto& appName = tokens.front();
    out << appName;

    // Options can have multiple names ("h", "help"). Limit to unique
    // result entries, keeping the longest name.
    std::unordered_map<std::shared_ptr<Result>, std::string> values;
    for (const auto& entry : results_) {
        const auto& name = entry.first;
        const auto& value = entry.second;

        auto found = values.find(value);
        if (found == values.cend())
            values.insert({value, name});
        else {
            if (found->second.length() < name.length()) found->second = name;
        }
    }

    // Option parameter output
    for (const auto& entry : values) {
        const auto* result = entry.first.get();
        const auto& name = entry.second;
        const auto typeIndex = result->which();

        const bool shortOption = (name.size() == 1);
        const std::string dashStyle = (shortOption ? " -" : " --");

        // print enabled switches
        if (typeIndex == 9) {
            const bool isSet = *result;
            if (isSet) out << dashStyle << name;
        }

        // print non-empty string parameters
        else if (typeIndex == 10) {
            const std::string stringValue = *result;
            if (!stringValue.empty()) out << dashStyle << name << '=' << stringValue;
        }

        // for all numeric types
        else
            out << dashStyle << name << '=' << *result;
    }

    //  Positional arg output
    for (const auto& posArg : posArgValues_)
        out << ' ' << posArg;

    return out.str();
}

const std::string& Results::InputCommandLine() const { return inputCommandLine_; }

Results& Results::InputCommandLine(std::string cmdLine)
{
    inputCommandLine_ = std::move(cmdLine);
    return *this;
}

std::string Results::LogFile() const
{
    const auto& logFileOpt = (*this)[Builtin::LogFile];
    const std::string logFileStr = logFileOpt;
    return logFileStr;
}

PacBio::Logging::LogLevel Results::LogLevel() const
{
    const auto& logLevelOpt = (*this)[Builtin::LogLevel];
    const std::string logLevelStr = logLevelOpt;
    return PacBio::Logging::LogLevel(logLevelStr);
}

size_t Results::NumThreads() const
{
    const auto& numThreadOpt = (*this)[Builtin::NumThreads];
    const unsigned int requestedNumThreads = numThreadOpt;
    // NOTE: max may be 0 if unknown
    const unsigned int maxNumThreads = std::thread::hardware_concurrency();

    if (requestedNumThreads == 0) return std::max(1U, maxNumThreads);
    if (maxNumThreads == 0) return requestedNumThreads;
    return std::min(requestedNumThreads, maxNumThreads);
}

const std::vector<std::string>& Results::PositionalArguments() const { return posArgValues_; }

Results& Results::PositionalArguments(const std::vector<internal::PositionalArgumentData>& posArgs)
{
    for (const auto& posArg : posArgs)
        posArgNames_.push_back(posArg.name);
    return *this;
}

bool Results::Verbose() const
{
    const auto found = results_.find("verbose");
    if (found != results_.cend()) return *(found->second.get());
    return false;  // verbose option not enabled
}

const Result& Results::operator[](const Option& opt) const
{
    const auto optionNames = internal::OptionTranslator::OptionNames(opt);
    for (const auto& name : optionNames) {
        const auto found = results_.find(name);
        if (found != results_.cend()) return *(found->second.get());
    }

    // not found
    std::ostringstream out;
    out << "[pbcopper] command line results ERROR: unknown option, with name(s):\n";
    for (const auto& name : optionNames)
        out << "  " << name << '\n';
    throw std::invalid_argument{out.str()};
}

const std::string& Results::operator[](const PositionalArgument& posArg) const
{
    const auto& name = internal::PositionalArgumentTranslator::PositionalArgName(posArg);
    for (size_t i = 0; i < posArgNames_.size(); ++i) {
        if (posArgNames_.at(i) == name) return posArgValues_.at(i);
    }

    // not found
    std::ostringstream out;
    out << "[pbcopper] command line results ERROR: unknown positional argument, with name: " << name
        << '\n';
    throw std::invalid_argument{out.str()};
}

}  // namespace CLI_v2
}  // namespace PacBio
