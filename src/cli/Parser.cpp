// Author: Derek Barnett

#include <pbcopper/cli/Parser.h>

#include <cassert>
#include <unordered_map>

#include <pbcopper/cli/Interface.h>

namespace PacBio {
namespace CLI {

class Parser::ParserPrivate
{
public:
    using NameLookup = std::unordered_map<std::string, size_t>;
    using ValueLookup = std::unordered_map<size_t, std::vector<std::string>>;

    // registered interface
    PacBio::CLI::Interface interface_;
    PacBio::CLI::Results results_;

    std::vector<std::string> observedOptions_;  // consider a set
    ValueLookup observedOptionValues_;

    explicit ParserPrivate(const Interface& interface) : interface_(interface), results_(interface)
    {
    }

    ParserPrivate(const ParserPrivate&) = default;

    void Parse(const std::vector<std::string>& args);
    void ParseOptionValue(const std::string& optionName, const std::string& argument,
                          std::vector<std::string>::const_iterator* argumentIterator,
                          std::vector<std::string>::const_iterator argsEnd);
};

void Parser::ParserPrivate::Parse(const std::vector<std::string>& args)
{
    results_ = Results{interface_, args};

    const auto doubleDash = "--";
    const auto dash = '-';
    const auto equal = '=';
    auto forcePositional = false;

    SingleDashMode singleDashMode_ = SingleDashMode::ParseAsShortOptions;

    // skip app name
    if (args.empty())
        throw std::runtime_error(
            "CLI::Parser - received an empty argument list (should have at least the program "
            "name)");
    auto argIter = args.cbegin();
    auto argEnd = args.cend();
    ++argIter;

    // loop over args
    for (; argIter != argEnd; ++argIter) {
        const auto& arg = *argIter;

        const auto foundDoubleDash = (arg.find(doubleDash) == 0);
        const auto foundSingleDash = (arg.find(dash) == 0);
        const auto isPositional = !(foundDoubleDash || foundSingleDash);

        // positional arg
        if (isPositional || forcePositional) results_.RegisterPositionalArg(arg);

        // long option
        else if (foundDoubleDash) {
            if (arg.length() > 2) {

                // pull option name from arg
                auto optionName = arg.substr(2);  // strip '--'
                const auto equalOffset = optionName.find(equal);
                if (equalOffset != std::string::npos)
                    optionName = optionName.substr(0, equalOffset);

                // register found & parse value
                const auto optionId = interface_.IdForOptionName(optionName);
                results_.RegisterObservedOption(optionId);
                ParseOptionValue(optionName, arg, &argIter, argEnd);
            } else
                forcePositional = true;
        }

        // short option
        else {
            assert(foundSingleDash);

            // single dash only (probably indicating 'stdin')
            if (arg.length() == 1) {
                results_.RegisterPositionalArg(arg);
                continue;
            }

            switch (singleDashMode_) {
                case SingleDashMode::ParseAsShortOptions: {
                    auto optionName = std::string{};
                    auto valueFound = false;

                    for (size_t i = 1; i < arg.size(); ++i) {
                        optionName = arg.substr(i, 1);

                        const auto optionId = interface_.IdForOptionName(optionName);
                        results_.RegisterObservedOption(optionId);
                        const auto expectsValue = interface_.ExpectsValue(optionName);
                        if (expectsValue) {
                            if (i + 1 < arg.size()) {
                                if (arg.at(i + 1) == equal) ++i;
                                results_.RegisterOptionValue(optionId, arg.substr(i + 1));
                                valueFound = true;
                            }
                            break;
                        }
                        if (i + 1 < arg.size() && arg.at(i + 1) == equal) break;
                    }

                    if (!valueFound) ParseOptionValue(optionName, arg, &argIter, argEnd);
                    break;
                }

                case SingleDashMode::ParseAsLongOptions: {
                    auto optionName = std::string{""};  // strip -, get up to "="
                    const auto optionId = interface_.IdForOptionName(optionName);
                    results_.RegisterObservedOption(optionId);
                    ParseOptionValue(optionName, arg, &argIter, argEnd);
                    break;
                }
                default:
                    throw std::runtime_error{"CLI::Parser - unexpected dash mode"};
            }
        }

        // make sure we get out at end
        if (argIter == argEnd) break;
    }
}

void Parser::ParserPrivate::ParseOptionValue(
    const std::string& optionName, const std::string& argument,
    std::vector<std::string>::const_iterator* argumentIterator,
    std::vector<std::string>::const_iterator argsEnd)
{
    if (!interface_.HasOptionRegistered(optionName))
        throw std::runtime_error("CLI::Parser - unexpected argument " + argument);

    const auto equal = '=';
    const auto equalPos = argument.find(equal);

    const auto expectsValue = interface_.ExpectsValue(optionName);
    if (expectsValue) {
        const auto optionId = interface_.IdForOptionName(optionName);
        std::string value;
        if (equalPos == std::string::npos) {
            ++(*argumentIterator);
            if (*argumentIterator == argsEnd)
                throw std::runtime_error("CLI::Parser - missing value after " + argument);
            value = *(*argumentIterator);
        } else
            value = argument.substr(equalPos + 1);

        // register value for option
        results_.RegisterOptionValueString(optionId, value);

    } else {
        if (equalPos != std::string::npos)
            throw std::runtime_error("CLI::Parser - unexpected value after " +
                                     argument.substr(0, equalPos));
    }
}

Parser::Parser(const Interface& interface) : d_(new ParserPrivate{interface}) {}

Parser::Parser(const Parser& other) : d_(new ParserPrivate{*other.d_.get()}) {}

Parser::~Parser(void) {}

Results Parser::Parse(const std::vector<std::string>& args)
{
    d_->Parse(args);
    return d_->results_;
}

Results Parser::Parse(int argc, char* argv[])
{
    return Parse(std::vector<std::string>{argv, argv + argc});
}

}  // namespace CLI
}  // namespace PacBio
