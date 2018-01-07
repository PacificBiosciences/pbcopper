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

#include <pbcopper/cli/Option.h>
#include <stdexcept>

namespace PacBio {
namespace CLI {
namespace internal {

static const Option defaultHelpOption {
    "help",
    {"h", "help"},
    "Output this help."
};

static const Option defaultLogLevelOption {
    "log_level",
    {"log-level", "logLevel"},
    "Set log level.",
    Option::StringType("INFO")
};

static const Option defaultVerboseOption {
    "verbose",
    {"v", "verbose"},
    "Use verbose output."
};

static const Option defaultVersionOption {
    "version",
    "version",
    "Output version info."
};

} // namespace internal

const Option& Option::DefaultHelpOption(void)
{ return internal::defaultHelpOption; }

const Option& Option::DefaultLogLevelOption(void)
{ return internal::defaultLogLevelOption; }

const Option& Option::DefaultVerboseOption(void)
{ return internal::defaultVerboseOption; }

const Option& Option::DefaultVersionOption(void)
{ return internal::defaultVersionOption; }

std::string Option::TypeId(void) const
{
    using Json = PacBio::JSON::Json;

    const bool hasChoices = HasChoices();
    const auto& type = DefaultValue().type();
    switch (type)
    {
        case Json::value_t::number_integer  : // fall through
        case Json::value_t::number_unsigned :
        {
            return (hasChoices ? "choice_integer" : "integer");
        }

        case Json::value_t::number_float :
        {
            return (hasChoices ? "choice_float" : "float");
        }
        case Json::value_t::string :
        {
            return (hasChoices ? "choice_string" : "string");
        }
        case Json::value_t::boolean :
        {
            return "boolean";
        }

        // unsupported/invalid JSON types
        case Json::value_t::array     : // fall through
        case Json::value_t::null      : // .
        case Json::value_t::object    : // .
        case Json::value_t::discarded : // .
        default:
            throw std::runtime_error{ "PacBio::CLI::ToolContract::JsonPrinter - unknown type for option: "+ Id() };
    }
}

} // namespace CLI
} // namespace PacBio
