#include <pbcopper/cli2/internal/CommandLineParser.h>

#include <cassert>
#include <deque>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>

#include <pbcopper/utility/StringUtils.h>

using OptionData = PacBio::CLI_v2::internal::OptionData;
using OptionValue = PacBio::CLI_v2::OptionValue;
using OptionValueType = PacBio::CLI_v2::OptionValueType;
using Results = PacBio::CLI_v2::Results;

namespace {

constexpr static const char token_doubleDash[] = "--";
constexpr static const char token_dash = '-';
constexpr static const char token_equal = '=';

struct CommandLineParserException : public std::runtime_error
{
    CommandLineParserException(const std::string& reason) : std::runtime_error("")
    {
        std::ostringstream msg;
        msg << "[pbcopper] command line parser ERROR: " << reason << '\n';
        msg_ = msg.str();
    }

    const char* what() const noexcept override { return msg_.c_str(); }

    std::string msg_;
};

bool IsTrueBoolean(const std::string& s)
{
    return boost::iequals(s, "true") || boost::iequals(s, "on");
}

bool IsFalseBoolean(const std::string& s)
{
    return boost::iequals(s, "false") || boost::iequals(s, "off");
}

OptionValue ValueFromString(const OptionData& option, const std::string valueString)
{
    switch (option.type) {
        case OptionValueType::INT: {
            const int64_t v = std::strtoll(valueString.c_str(), nullptr, 10);
            return OptionValue{v};
        }

        case OptionValueType::UINT: {
            const uint64_t v = std::strtoull(valueString.c_str(), nullptr, 10);
            return OptionValue{v};
        }

        case OptionValueType::FLOAT:
            return OptionValue{std::strtod(valueString.c_str(), nullptr)};

        case OptionValueType::STRING:
        case OptionValueType::FILE:
        case OptionValueType::DIR:
            return OptionValue{valueString};

        case OptionValueType::BOOL: {
            if (IsTrueBoolean(valueString)) return OptionValue{true};
            if (IsFalseBoolean(valueString)) return OptionValue{false};
            throw CommandLineParserException{"value '" + valueString +
                                             "' is unexpected after a switch"};
        }
        default:
            throw CommandLineParserException{"unknown option type"};
    }
}

}  // namespace

namespace PacBio {
namespace CLI_v2 {
namespace internal {

CommandLineParser::CommandLineParser(Interface interface) : interface_{std::move(interface)}
{
    options_ = interface_.Options();
    for (const auto& option : options_) {
        for (const auto& name : option.names)
            optionsByName_.insert({name, option});
        for (const auto& hiddenName : option.hiddenNames)
            optionsByName_.insert({hiddenName, option});
    }
}

const OptionData& CommandLineParser::OptionFor(const std::string& name) const
{
    const auto found = optionsByName_.find(name);
    if (found != optionsByName_.cend()) return found->second;
    throw CommandLineParserException{"unknown option '" + name + "'"};
}

Results CommandLineParser::Parse(const std::vector<std::string>& arguments) const
{
    if (arguments.empty())
        throw CommandLineParserException{
            "empty argument list (should have at least the program name)"};

    Results results = interface_.MakeDefaultResults();
    results.InputCommandLine(PacBio::Utility::Join(arguments, " "));

    // (+1 to skip application name)
    std::deque<std::string> args{arguments.cbegin() + 1, arguments.cend()};
    while (!args.empty()) {
        const auto arg = args.front();
        args.pop_front();

        // bare '--' signifies the end of options & start of posArgs
        if (arg == "--") break;

        const auto isLongOption = (arg.find(token_doubleDash) == 0);
        const auto isShortOption = (arg.find(token_dash) == 0);

        // long option (--reference)
        if (isLongOption) ParseLongOption(arg, args, results);

        // short option (-r)
        else if (isShortOption)
            ParseShortOption(arg, args, results);

        // positional argument
        else
            results.AddPositionalArgument(arg);
    }

    // add any remaining positional args after '--' break
    while (!args.empty()) {
        results.AddPositionalArgument(args.front());
        args.pop_front();
    }

    return results;
}

void CommandLineParser::ParseLongOption(const std::string& arg, std::deque<std::string>& args,
                                        Results& results) const
{
    assert(arg.length() > 2);

    // strip leading '--'
    const auto optionToken = arg.substr(2);

    std::string optionName;
    std::string valueString;

    // if arg is "opt=value", do the simple split & store
    const auto equalOffset = optionToken.find(token_equal);
    if (equalOffset != std::string::npos) {
        optionName = optionToken.substr(0, equalOffset);
        valueString = optionToken.substr(equalOffset + 1);

        const auto& option = OptionFor(optionName);
        auto value = ValueFromString(option, valueString);
        results.AddObservedValue(optionName, value, SetByMode::USER);
        return;
    }

    // arg is either "opt" or "opt value", ignore parsing out the "option token"
    optionName = optionToken;

    // boolean options are a special case
    const auto& option = OptionFor(optionName);
    if (option.type == OptionValueType::BOOL) results.AddObservedFlag(optionName, SetByMode::USER);

    // non-boolean options
    else {
        const bool argsRemaining = !args.empty();
        if (argsRemaining) {
            valueString = args.front();

            const auto nextIsOption = (valueString.find(token_dash) == 0);

            // if the expected value is actually the next option
            if (nextIsOption)
                throw CommandLineParserException{"value is missing for option '" + optionName +
                                                 "'"};
            else {
                auto value = ValueFromString(option, valueString);
                results.AddObservedValue(optionName, value, SetByMode::USER);
                args.pop_front();
            }
        } else
            throw CommandLineParserException{"value is missing for option '" + optionName + "'"};
    }
}

void CommandLineParser::ParseShortOption(const std::string& arg, std::deque<std::string>& args,
                                         Results& results) const
{
    // single dash only (usually indicating 'stdin')
    if (arg.length() == 1) {
        results.AddPositionalArgument(arg);
        return;
    }

    std::string optionName;
    bool valueExpected = true;

    for (size_t i = 1; i < arg.size(); ++i) {
        optionName = arg.substr(i, 1);

        const auto& option = OptionFor(optionName);
        if (option.type == OptionValueType::BOOL) {
            results.AddObservedFlag(optionName, SetByMode::USER);
            valueExpected = false;
        } else {
            if (i + 1 < arg.size()) {
                if (arg.at(i + 1) == token_equal) ++i;
                auto valueString = arg.substr(i + 1);
                auto value = ValueFromString(option, valueString);
                results.AddObservedValue(optionName, value, SetByMode::USER);
                valueExpected = false;
            }
        }
    }

    if (valueExpected) {
        assert(!optionName.empty());

        auto valueString = args.front();
        args.pop_front();

        const auto& option = OptionFor(optionName);
        auto value = ValueFromString(option, valueString);
        results.AddObservedValue(optionName, value, SetByMode::USER);
    }
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
