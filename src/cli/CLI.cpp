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

#include <pbcopper/cli/CLI.h>

#include <pbcopper/cli/HelpPrinter.h>
#include <pbcopper/cli/Parser.h>
#include <pbcopper/cli/VersionPrinter.h>
#include <pbcopper/cli/toolcontract/JsonPrinter.h>
#include <pbcopper/cli/toolcontract/ResolvedToolContract.h>
#include <fstream>
#include <iostream>
#include <cstdlib>

namespace PacBio {
namespace CLI {

int Run(int argc, char *argv[],
        const Interface& interface,
        const ResultsHandler& handler)
{
    return Run(std::vector<std::string>{argv, argv + argc},
               interface,
               handler);
}

int Run(const std::vector<std::string>& args,
        const Interface& interface,
        const ResultsHandler& handler)
{
    // parse command line args
    Parser parser(interface);
    const auto results = parser.Parse(args);

    // check for help option (enabled & requested)
    if (interface.HasHelpOptionRegistered()) {
        const auto helpOptionId = interface.HelpOption().Id();
        const bool helpRequested = results[helpOptionId];
        if (helpRequested) {
            HelpPrinter::Print(interface, std::cout);
            return EXIT_SUCCESS;
        }
    }

    // check for version option (enabled & requested)
    if (interface.HasVersionOptionRegistered()) {
        const auto versionOptionId = interface.VersionOption().Id();
        const bool versionRequested = results[versionOptionId];
        if (versionRequested) {
            VersionPrinter::Print(interface, std::cout);
            return EXIT_SUCCESS;
        }
    }

    // tool contract support
    if (interface.IsToolContractEnabled()) {

        // check for emit-tool-contract requested
        const bool emitTcRequested = results["emit_tc"];
        if (emitTcRequested) {
            ToolContract::JsonPrinter::Print(interface, std::cout);
            return EXIT_SUCCESS;
        }

        // check for resolved tool contract cmdline input
        const std::string& rtcFn = results["rtc_provided"];
        if (!rtcFn.empty()) {

            // invoke app callback with results parsed from RTC
            ToolContract::ResolvedToolContract rtc(interface);
            std::ifstream in(rtcFn);
            if (in) {
                const auto rtcResults = rtc.Parse(in);
                return handler(rtcResults);
            }
        }
    }

    // otherwise invoke app callback with results parsed from command line
    return handler(results);
}

void PrintHelp(const Interface& interface, std::ostream& out)
{
    HelpPrinter::Print(interface, out);
}

void PrintVersion(const Interface& interface, std::ostream& out)
{
    VersionPrinter::Print(interface, out);
}

} // namespace CLI
} // namespace PacBio
