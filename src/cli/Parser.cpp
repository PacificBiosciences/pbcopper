// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Derek Barnett

#include <pbcopper/cli/Parser.h>

#include <cassert>
#include <unordered_map>

#include <pbcopper/cli/Interface.h>

namespace PacBio {
namespace CLI {
namespace internal {

class ParserPrivate
{
public:
    typedef std::unordered_map<std::string, size_t>                NameLookup;
    typedef std::unordered_map<size_t, std::vector<std::string> >  ValueLookup;

public:
    // registered interface
    PacBio::CLI::Interface interface_;
    PacBio::CLI::Results   results_;

    std::vector<std::string> observedOptions_;  // consider a set
    ValueLookup observedOptionValues_;

public:
    ParserPrivate(const Interface& interface)
        : interface_(interface)
        , results_(interface)
    { }

    ParserPrivate(const ParserPrivate& other) = default;

    void Parse(const std::vector<std::string>& args);
    void ParseOptionValue(const std::string& optionName,
                          const std::string& argument,
                          std::vector<std::string>::const_iterator* argumentIterator,
                          std::vector<std::string>::const_iterator argsEnd);
};

void ParserPrivate::Parse(const std::vector<std::string>& args)
{
    results_ = Results{ interface_, args };

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

        const auto foundDoubleDash = (arg.find(doubleDash) == 0);
        const auto foundSingleDash = (arg.find(dash) == 0);
        const auto isPositional = !(foundDoubleDash || foundSingleDash);

        // positional arg
        if (isPositional || forcePositional)
            results_.RegisterPositionalArg(arg);

        // long option
        else if (foundDoubleDash) {
            if (arg.length() > 2) {

                // pull option name from arg
                auto optionName = arg.substr(2); // strip '--'
                const auto equalOffset = optionName.find(equal);
                if (equalOffset != std::string::npos)
                    optionName = optionName.substr(0, equalOffset);

                // register found & parse value
                const auto& optionId = interface_.IdForOptionName(optionName);
                results_.RegisterObservedOption(optionId);
                ParseOptionValue(optionName, arg, &argIter, argEnd);
            }
            else forcePositional = true;
        }

        // short option
        else {
            assert(foundSingleDash);

            // single dash only (probably indicating 'stdin')
            if (arg.length() == 1) {
                results_.RegisterPositionalArg(arg);
                continue;
            }

            switch (singleDashMode_)
            {
                case SingleDashMode::ParseAsShortOptions :
                {
                    auto optionName = std::string{ };
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
                                results_.RegisterOptionValue(optionId, arg.substr(i+1));
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
                    auto optionName = std::string{ "" }; // strip -, get up to "="
                    const auto& optionId = interface_.IdForOptionName(optionName);
                    results_.RegisterObservedOption(optionId);
                    ParseOptionValue(optionName, arg, &argIter, argEnd);
                    break;
                }
            }
        }

        // make sure we get out at end
        if (argIter == argEnd)
            break;
    }
}

void ParserPrivate::ParseOptionValue(const std::string& optionName,
                                     const std::string& argument,
                                     std::vector<std::string>::const_iterator* argumentIterator,
                                     std::vector<std::string>::const_iterator argsEnd)
{
    if (!interface_.HasOptionRegistered(optionName))
        throw std::runtime_error("CLI::Parser - unexpected argument " + argument);

    const auto equal = '=';
    const auto equalPos = argument.find(equal);

    const auto expectsValue = interface_.ExpectsValue(optionName);
    if (expectsValue) {
        const auto& optionId = interface_.IdForOptionName(optionName);
        std::string value;
        if (equalPos == std::string::npos) {
            ++(*argumentIterator);
            if (*argumentIterator == argsEnd)
                throw std::runtime_error("CLI::Parser - missing value after " + argument);
            value = *(*argumentIterator);
        } else
            value = argument.substr(equalPos+1);

        // register value for option
        results_.RegisterOptionValueString(optionId, value);

    } else {
        if (equalPos != std::string::npos)
            throw std::runtime_error("CLI::Parser - unexpected value after "+argument.substr(0, equalPos));
    }
}

} // namespace internal

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

Results Parser::Parse(const std::vector<std::string>& args)
{
    d_->Parse(args);
    return d_->results_;
}

Results Parser::Parse(int argc, char *argv[])
{ return Parse(std::vector<std::string>{argv, argv + argc}); }

} // namespace CLI
} // namespace PacBio
