#include "pbcopper/cli/Parser.h"
#include "pbcopper/cli/Interface.h"
#include <unordered_map>
#include <cassert>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

namespace PacBio {
namespace CLI {
namespace internal {

class ParserPrivate
{
public:
    typedef unordered_map<string, size_t>           NameLookup;
    typedef unordered_map<size_t, vector<string> >  ValueLookup;

public:
    // registered interface
    PacBio::CLI::Interface interface_;
    PacBio::CLI::Results   results_;

    vector<string> observedOptions_;              // consider a set
    ValueLookup    observedOptionValues_;

public:
    ParserPrivate(const Interface& interface)
        : interface_(interface)
        , results_(interface)
    { }

    ParserPrivate(const ParserPrivate& other) = default;

    void Parse(const vector<string>& args);
    void ParseOptionValue(const string& optionName,
                          const string& argument,
                          vector<string>::const_iterator* argumentIterator,
                          vector<string>::const_iterator argsEnd);
};

void ParserPrivate::Parse(const vector<string>& args)
{
    const auto doubleDash = "--";
    const auto dash  = '-';
    const auto equal = '=';
    auto forcePositional = false;

    SingleDashMode singleDashMode_ = SingleDashMode::ParseAsShortOptions;

    // skip app name
    if (args.empty())
        throw std::runtime_error("CLI::Parser - received an empty argument list (should have at least the program name)");
    auto argIter = args.cbegin();
    auto argEnd  = args.cend();
    ++argIter;

    // loop over args
    for (; argIter != argEnd; ++argIter) {
        const auto& arg = *argIter;

        // definitely positional
        if (forcePositional)
            results_.RegisterPositionalArg(arg);

        // long option
        else if (arg.find(doubleDash) == 0) {
            if (arg.length() > 2) {

                // pull option name from arg
                auto optionName = arg.substr(2); // strip '--'
                const auto equalOffset = optionName.find(equal);
                if (equalOffset != string::npos)
                    optionName = optionName.substr(0, equalOffset);

                // register found & parse value
                const auto& optionId = interface_.IdForOptionName(optionName);
                results_.RegisterObservedOption(optionId);
                ParseOptionValue(optionName, arg, &argIter, argEnd);

            }
            else forcePositional = true;
        }

        // short option
        else if (arg.find(dash) == 0) {

            // single dash only (probably indicating 'stdin')
            if (arg.length() == 1) {
                results_.RegisterPositionalArg(arg);
                continue;
            }

            switch (singleDashMode_)
            {
                case SingleDashMode::ParseAsShortOptions :
                {
                    auto optionName = string{ };
                    auto valueFound = false;

                    for (size_t i = 1; i < arg.size(); ++i) {
                        optionName = arg.substr(i, 1);

                        const auto& optionId = interface_.IdForOptionName(optionName);
                        results_.RegisterObservedOption(optionId);
                        const auto expectsValue = interface_.ExpectsValue(optionName);
                        if (expectsValue) {
                            if (i+1 < arg.size()) {
                                if (arg.at(i+1) == equal)
                                    ++i;
                                results_.RegisterOptionValue(optionName,  arg.substr(i+1));
                                valueFound = true;
                            }
                            break;
                        }
                        if (i+1 < arg.size() && arg.at(i+1) == equal)
                            break;
                    }

                    if (!valueFound)
                        ParseOptionValue(optionName, arg, &argIter, argEnd);
                    break;
                }

                case SingleDashMode::ParseAsLongOptions :
                {
                    auto optionName = string{ "" }; // strip -, get up to "="
                    const auto& optionId = interface_.IdForOptionName(optionName);
                    results_.RegisterObservedOption(optionId);
                    ParseOptionValue(optionName, arg, &argIter, argEnd);
                    break;
                }
            }
        }

        // positional argument
        else
            results_.RegisterPositionalArg(arg);

        // make sure we get out at end
        if (argIter == argEnd)
            break;
    }
}

void ParserPrivate::ParseOptionValue(const string& optionName,
                                     const string& argument,
                                     vector<string>::const_iterator* argumentIterator,
                                     vector<string>::const_iterator argsEnd)
{
    if (!interface_.HasOptionRegistered(optionName))
        throw std::runtime_error("CLI::Parser - unexpected argument " + argument);

    const auto equal = '=';
    const auto equalPos = argument.find(equal);

    const auto expectsValue = interface_.ExpectsValue(optionName);
    if (expectsValue) {
        const auto& optionId = interface_.IdForOptionName(optionName);
        if (equalPos == string::npos) {
            ++(*argumentIterator);
            if (*argumentIterator == argsEnd)
                throw std::runtime_error("CLI::Parser - missing value after " + argument);
            results_.RegisterOptionValueString(optionId, *(*argumentIterator));
        } else
            results_.RegisterOptionValueString(optionId, argument.substr(equalPos+1));
    } else {
        if (equalPos != string::npos)
            throw std::runtime_error("CLI::Parser - unexpected value after "+argument.substr(0, equalPos));
    }
}

} // namespace internal
} // namespace CLI
} // namespace PacBio

// ------------------------
// PacBio::CLI::Parser
// ------------------------

Parser::Parser(const Interface &interface)
    : d_(new internal::ParserPrivate{ interface })
{ }

Parser::Parser(const Parser& other)
    : d_(new internal::ParserPrivate{*other.d_.get()})
{ }

Parser::~Parser(void) { }

Results Parser::Parse(const vector<string>& args)
{
    d_->Parse(args);
    return d_->results_;
}

Results Parser::Parse(int argc, char *argv[])
{ return Parse(vector<string>{argv, argv + argc}); }
