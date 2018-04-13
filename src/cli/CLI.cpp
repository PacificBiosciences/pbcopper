// Author: Derek Barnett

#include <pbcopper/cli/CLI.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <pbcopper/cli/HelpPrinter.h>
#include <pbcopper/cli/Parser.h>
#include <pbcopper/cli/VersionPrinter.h>
#include <pbcopper/cli/toolcontract/JsonPrinter.h>
#include <pbcopper/cli/toolcontract/ResolvedToolContract.h>

namespace PacBio {
namespace CLI {

int Run(int argc, char* argv[], const Interface& interface, const ResultsHandler& handler)
{
    return Run(std::vector<std::string>{argv, argv + argc}, interface, handler);
}

int Run(const std::vector<std::string>& args, const Interface& interface,
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

}  // namespace CLI
}  // namespace PacBio
