#include <pbcopper/cli2/CLI.h>

#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/CommandLineParser.h>
#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>
#include <pbcopper/cli2/internal/MultiToolInterfaceHelpPrinter.h>
#include <pbcopper/cli2/internal/VersionPrinter.h>
#include <pbcopper/logging/Logging.h>
#include <pbcopper/utility/Alarm.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include <cassert>
#include <cstdlib>

using CommandLineParser = PacBio::CLI_v2::internal::CommandLineParser;
using InterfaceHelpPrinter = PacBio::CLI_v2::internal::InterfaceHelpPrinter;
using MultiToolInterfaceHelpPrinter = PacBio::CLI_v2::internal::MultiToolInterfaceHelpPrinter;

namespace PacBio {
namespace CLI_v2 {

bool LogLevelIsUserProvided(const Interface& interface, const Results& results)
{
    const auto& logLevelOption = interface.LogLevelOption();
    if (!logLevelOption) {
        return false;
    }

    assert(!logLevelOption->Names.empty());
    const auto& logLevelResult = results[Builtin::LogLevel];
    return logLevelResult.IsUserProvided();
}

int Run(int argc, char* argv[], const Interface& interface, const ResultsHandler& handler)
{
    return Run(std::vector<std::string>{argv, argv + argc}, interface, handler);
}

int Run(const std::vector<std::string>& args, const Interface& interface,
        const ResultsHandler& handler)
{
    //
    // If application name only & positional arguments are required, show help.
    // This allows, for example, tools that to stream stdin without requiring
    // input filenames.
    //
    if (args.size() == 1 && interface.HasRequiredPosArgs()) {
        const InterfaceHelpPrinter help{interface, internal::HiddenOptionMode::HIDE};
        std::cout << help;
        return EXIT_SUCCESS;
    }

    // parse command line args
    CommandLineParser parser{interface};
    Results results = parser.Parse(args);

    // help
    if (results[Builtin::Help] || results[Builtin::ShowAllHelp]) {
        const internal::HiddenOptionMode showHiddenOptions = results[Builtin::ShowAllHelp]
                                                                 ? internal::HiddenOptionMode::SHOW
                                                                 : internal::HiddenOptionMode::HIDE;
        const InterfaceHelpPrinter help{interface, showHiddenOptions};
        std::cout << help;
        return EXIT_SUCCESS;
    }

    // version
    if (results[Builtin::Version]) {
        interface.PrintVersion();
        return EXIT_SUCCESS;
    }

    const std::string alarmsOutputFilename{results.AlarmsFile()};
    const bool allowExceptionsPassthrough{results.ExceptionPassthrough()};

    //
    // Initialize logging
    //
    // Setup logging configuration. Priority is given in the following (decreasing) order:
    //  * command line-provided options
    //  * multi-tool interface config
    //  * interface config
    //
    // If 'log-level' or 'log-file' were set via command line, use it. Otherwise,
    // use the application's default config provided.
    //
    // Also ensures that the Results object passed to the application runner
    // reflects the actual, active log level.
    //
    Logging::LogConfig logConfig = interface.LogConfig();
    if (LogLevelIsUserProvided(interface, results)) {
        logConfig.Level = results.LogLevel();
    } else {
        results.AddObservedValue("log-level", logConfig.Level.ToString(), SetByMode::DEFAULT);
    }

    const auto logger = [&]() {
        if (interface.LogFileOption()) {
            const auto logFile = results.LogFile();
            if (!logFile.empty()) {
                return std::make_unique<Logging::Logger>(logFile, logConfig);
            }
        }
        return std::make_unique<Logging::Logger>(std::cerr, logConfig);
    }();

    Logging::Logger::Current(logger.get());

    // bypass exception logging/alarm on exit, if requested
    if (allowExceptionsPassthrough) {
        return handler(results);
    }

    const auto& applicationName = interface.ApplicationName();

    auto printToAlarmFile = [&alarmsOutputFilename, &applicationName](const Utility::Alarm alarm) {
        std::ofstream alarmsOutput{alarmsOutputFilename};
        if (alarmsOutput) {
            Utility::Alarm::WriteAlarms(alarmsOutput, {alarm}, applicationName);
        } else {
            PBLOG_FATAL << "Could not rewrite alarms JSON to " << alarmsOutputFilename;
        }
    };

    // run application
    try {
        return handler(results);
    } catch (const Utility::AlarmException& a) {
        Logging::LogMessage(a.SourceFilename(), a.FunctionName(), a.LineNumber(),
                            Logging::LogLevel::FATAL, Logging::Logger::Current())
            << applicationName << " ERROR: " << a.Message();
        if (!alarmsOutputFilename.empty()) {
            printToAlarmFile(
                Utility::Alarm{a.Name(), a.Message(), a.Severity(), a.Info(), a.Exception()});
        }
    } catch (const std::exception& e) {
        PBLOG_FATAL << applicationName << " ERROR: " << e.what();
        if (!alarmsOutputFilename.empty()) {
            printToAlarmFile(
                Utility::Alarm{applicationName, e.what(), "FATAL", "", "std::exception"});
        }
    } catch (...) {
        PBLOG_FATAL << "caught unknown exception type";
        if (!alarmsOutputFilename.empty()) {
            printToAlarmFile(Utility::Alarm{applicationName, "", "FATAL", "", "unknown exception"});
        }
    }

    return EXIT_FAILURE;
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
        const MultiToolInterfaceHelpPrinter help{interface, internal::HiddenOptionMode::HIDE};
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
        return Run(toolArgs, tool.interface, tool.runner);
    }

    //
    // command line has a second arg that is not a subtool, but could still be
    // top-level help/version
    //
    // "$ multi-tool -h"
    // "$ multi-tool --help"
    // "$ multi-tool --show-all-help"
    // "$ multi-tool --all-help"
    // "$ multi-tool --version"
    //
    // we have not done full command-line parsing (saving that for each subtool),
    // so do "manual" check here on special options
    //
    for (const auto& arg : args) {
        if ((arg == "-h") || (arg == "--help") || (arg == "--show-all-help") ||
            (arg == "--all-help")) {
            const internal::HiddenOptionMode showHiddenOptions =
                ((arg == "--show-all-help") || (arg == "--all-help"))
                    ? internal::HiddenOptionMode::SHOW
                    : internal::HiddenOptionMode::HIDE;
            const MultiToolInterfaceHelpPrinter help{interface, showHiddenOptions};
            std::cout << help;
            return EXIT_SUCCESS;
        } else if (arg == "--version") {
            interface.PrintVersion();
            return EXIT_SUCCESS;
        }
    }

    // no matching tool
    std::cerr << interface.ApplicationName()
              << " ERROR: [pbcopper] command line ERROR: unknown tool '" + args.at(1) +
                     "' requested\n";
    return EXIT_FAILURE;
}

}  // namespace CLI_v2
}  // namespace PacBio
