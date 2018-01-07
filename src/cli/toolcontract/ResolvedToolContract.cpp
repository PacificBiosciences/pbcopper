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

#include <pbcopper/cli/toolcontract/ResolvedToolContract.h>

#include <pbcopper/json/JSON.h>
#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {
namespace internal {

class RtcPrivate
{

public:
    const Interface interface_;

public:
    RtcPrivate(const Interface& interface)
        : interface_(interface)
    { }
};

} // namespace internal

ResolvedToolContract::ResolvedToolContract(const Interface& interface)
    : d_(new internal::RtcPrivate{ interface })
{ }

ResolvedToolContract::~ResolvedToolContract(void) { }

Results ResolvedToolContract::Parse(std::istream& in)
{
    using Json = PacBio::JSON::Json;

    const auto& interface = d_->interface_;
    const auto& task = interface.ToolContract().Task();

    Results results{ d_->interface_ };
    results.SetFromRTC(true);

    Json root(in);
    Json rtc = root["resolved_tool_contract"];

    // options (object)
    const Json options = rtc["options"];
    for (auto iter = options.cbegin(); iter != options.cend(); ++iter) {

        const auto& fullId = iter.key();
        const Json& value = iter.value();

        const auto idFields = PacBio::Utility::Split(fullId, '.');
        if (idFields.size() != 3)
            throw std::runtime_error("PacBio::CLI - unexpected option ID format: "+fullId);
        const auto optionId = idFields.at(2);

        results.RegisterOptionValue(optionId, value);
    }

    // log_level
    const Json log_level = rtc["log_level"];
    const std::string logLevelString = log_level;
    results.LogLevel(PacBio::Logging::LogLevel{logLevelString});

    // nproc
    const Json nproc = rtc["nproc"];
    results.NumProcessors(nproc);

    // input file (array)
    const Json inputFiles = rtc["input_files"];
    const auto inputFilesToOptions = task.InputFilesToOptions();
    const auto inputFilesToOptionsEnd = inputFilesToOptions.cend();
    for (size_t i = 0; i < inputFiles.size(); ++i) {
        const auto inputFile = inputFiles.at(i);
        const auto inputFilesToOptionsIter = inputFilesToOptions.find(i);
        if (inputFilesToOptionsIter == inputFilesToOptionsEnd)
            results.RegisterPositionalArg(inputFile);
        else {
            const auto optionId = inputFilesToOptionsIter->second;
            results.RegisterOptionValue(optionId, inputFile);
        }
    }

    // output file (array)
    const Json outputFiles = rtc["output_files"];
    const auto outputFilesToOptions = task.OutputFilesToOptions();
    const auto outputFilesToOptionsEnd = outputFilesToOptions.cend();
    for (size_t i = 0; i < outputFiles.size(); ++i) {
        const auto outputFile = outputFiles.at(i);
        const auto outputFilesToOptionsIter = outputFilesToOptions.find(i);
        if (outputFilesToOptionsIter == outputFilesToOptionsEnd)
            results.RegisterPositionalArg(outputFile);
        else {
            const auto optionId = outputFilesToOptionsIter->second;
            results.RegisterOptionValue(optionId, outputFile);
        }
    }

    return results;
}

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio
