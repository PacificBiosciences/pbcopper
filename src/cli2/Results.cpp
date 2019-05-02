#include <pbcopper/cli2/Results.h>

#include <stdexcept>

#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

namespace PacBio {
namespace CLI_v2 {

Results& Results::AddPositionalArgument(std::string arg)
{
    posArgValues_.push_back(std::move(arg));
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

const std::string& Results::InputCommandLine() const { return inputCommandLine_; }

Results& Results::InputCommandLine(std::string cmdLine)
{
    inputCommandLine_ = std::move(cmdLine);
    return *this;
}

PacBio::Logging::LogLevel Results::LogLevel() const
{

    const auto& logLevelOpt = (*this)[Builtin::LogLevel];
    const std::string logLevelStr = logLevelOpt;
    return PacBio::Logging::LogLevel(logLevelStr);
}

size_t Results::NumProcessors() const { return numProcessors_; }

Results& Results::NumProcessors(size_t nproc)
{
    numProcessors_ = nproc;
    return *this;
}

const std::vector<std::string>& Results::PositionalArguments() const { return posArgValues_; }

Results& Results::PositionalArguments(const std::vector<internal::PositionalArgumentData>& posArgs)
{
    for (const auto& posArg : posArgs)
        posArgNames_.push_back(posArg.name);
    return *this;
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