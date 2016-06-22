#include "pbcopper/cli/CLI.h"

#include "pbcopper/cli/HelpPrinter.h"
#include "pbcopper/cli/Parser.h"
#include "pbcopper/cli/VersionPrinter.h"
#include "pbcopper/cli/toolcontract/JsonPrinter.h"
#include "pbcopper/cli/toolcontract/ResolvedToolContract.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

namespace PacBio {
namespace CLI {

int Run(int argc, char *argv[],
        const Interface& interface,
        const ResultsHandler& handler)
{
    return Run(vector<string>{argv, argv + argc},
               interface,
               handler);
}

int Run(const vector<string>& args,
        const Interface& interface,
        const ResultsHandler& handler)
{
    // parse command line args
    Parser parser(interface);
    const auto results = parser.Parse(args);

    // check for built-in help (enabled by app & requested from cmdline)
    if (interface.IsBuiltInOptionEnabled(BuiltInOption::Help)) {
        const bool helpRequested = results["help"];
        if (helpRequested) {
            HelpPrinter::Print(interface, std::cout);
            return EXIT_SUCCESS;
        }
    }

    // check for built-in version (enabled by app & requested from cmdline)
    if (interface.IsBuiltInOptionEnabled(BuiltInOption::Version)) {
        const bool versionRequested = results["version"];
        if (versionRequested) {
            VersionPrinter::Print(interface, std::cout);
            return EXIT_SUCCESS;
        }
    }

    // check for built-in tool contract emission (enabled by app & requested from cmdline)
    if (interface.IsBuiltInOptionEnabled(BuiltInOption::EmitToolContract)) {
        const bool emitTcRequested = results["emit_tc"];
        if (emitTcRequested) {
            ToolContract::JsonPrinter::Print(interface, std::cout);
            return EXIT_SUCCESS;
        }
    }

    // check for resolved tool contract (enabled by app & provided from cmdline)
    if (interface.IsBuiltInOptionEnabled(BuiltInOption::ResolvedToolContract)) {
        const string& rtcFn = results["rtc_provided"];
        if (!rtcFn.empty()) {

            // invoke app callback with results parsed from RTC
            ToolContract::ResolvedToolContract rtc(interface);
            ifstream in(rtcFn);
            if (in) {
                const auto rtcResults = rtc.Parse(in);
                return handler(rtcResults);
            }
        }
    }

    // otherwise invoke app callback with results parsed from command line
    return handler(results);
}

void PrintHelp(const Interface& interface, ostream& out)
{
    HelpPrinter::Print(interface, out);
}

void PrintVersion(const Interface& interface, ostream& out)
{
    VersionPrinter::Print(interface, out);
}

} // namespace CLI
} // namespace PacBio
