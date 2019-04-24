#include <pbcopper/cli2/CLI.h>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/CommandLineParser.h>
#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>
#include <pbcopper/cli2/internal/MultiToolInterfaceHelpPrinter.h>
#include <pbcopper/cli2/internal/VersionPrinter.h>

using CommandLineParser = PacBio::CLI_v2::internal::CommandLineParser;
using InterfaceHelpPrinter = PacBio::CLI_v2::internal::InterfaceHelpPrinter;
using MultiToolInterfaceHelpPrinter = PacBio::CLI_v2::internal::MultiToolInterfaceHelpPrinter;
using VersionHelpPrinter = PacBio::CLI_v2::internal::VersionPrinter;

namespace PacBio {
namespace CLI_v2 {

int Run(int argc, char* argv[], const Interface& interface, const ResultsHandler& handler)
{
    return Run(std::vector<std::string>{argv, argv + argc}, interface, handler);
}

int Run(const std::vector<std::string>& args, const Interface& interface,
        const ResultsHandler& handler)
{
    // if just application name, show help
    //
    // TODO: what about piped in applications?
    //       $ some_upstream_tool | mytool
    //       revist if/when that happens
    //
    if (args.size() == 1) {
        const InterfaceHelpPrinter help{interface};
        std::cout << help;
        return EXIT_SUCCESS;
    }

    // parse command line args
    const CommandLineParser parser{interface};
    Results results = parser.Parse(args);

    // help
    if (results[Builtin::Help]) {
        const InterfaceHelpPrinter help{interface};
        std::cout << help;
        return EXIT_SUCCESS;
    }

    // version
    if (results[Builtin::Version]) {
        const VersionHelpPrinter version{interface};
        std::cout << version << '\n';
        return EXIT_SUCCESS;
    }

    //
    // TODO:
    //
    //  eventually set up logging here, with loglevel, logfile, ...
    //
    //  setup local stack logger here, using logLevel/logFile/etc params
    //  set global ref to point here
    //
    //  now with logger on stack here...
    //
    //  PBLOG_INFO << interface.ApplicationVersion();
    //  PBLOG_INFO << results.InputCommandLine();
    //
    //  int status = 0;
    //  try {
    //      status = handler(results);
    //  } catch (std::exception& e) {
    //      log exception message
    //      status = EXIT_FAILURE;
    //  }
    //  catch (... blah...) {
    //      something ??
    //      status = EXIT_FAILURE;
    //  }
    //  return status;
    //

    return handler(results);
}

int Run(int argc, char* argv[], const MultiToolInterface& interface)
{
    return Run(std::vector<std::string>{argv, argv + argc}, interface);
}

int Run(const std::vector<std::string>& args, const MultiToolInterface& interface)
{
    assert(!args.empty());

    //
    // if application name only, go straight to help
    //
    // "$ tool"
    //
    if (args.size() == 1) {
        const MultiToolInterfaceHelpPrinter help{interface};
        std::cout << help;
        return EXIT_SUCCESS;
    }

    //
    // otherwise fetch subtool interface, pop top-level application name, and run
    //
    // "$ tool subtool"
    // "$ tool subtool ..."
    //
    if (interface.HasTool(args.at(1))) {
        const Tool& tool = interface.ToolFor(args.at(1));
        const std::vector<std::string> toolArgs{args.cbegin() + 1, args.cend()};
        return Run(args, tool.interface, tool.runner);
    }

    //
    // command line has a second arg that is not a subtool, but could still be
    // top-level help/version
    //
    // "$ tool -h"
    // "$ tool --help"
    // "$ tool --version"
    //
    for (const auto& arg : args) {
        if (arg == "-h" || arg == "--help") {
            const MultiToolInterfaceHelpPrinter help{interface};
            std::cout << help;
            return EXIT_SUCCESS;
        } else if (arg == "--version") {
            const VersionHelpPrinter version{interface};
            std::cout << version << '\n';
            return EXIT_SUCCESS;
        }
    }

    // no matching tool
    throw std::runtime_error{"unknown tool: " + args.at(1)};
}

}  // namespace CLI_v2
}  // namespace PacBio
