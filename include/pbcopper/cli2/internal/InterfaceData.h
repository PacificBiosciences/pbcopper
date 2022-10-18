#ifndef PBCOPPER_CLI_v2_INTERFACEDATA_H
#define PBCOPPER_CLI_v2_INTERFACEDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/VersionPrinterCallback.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>
#include <pbcopper/cli2/internal/VersionPrinter.h>
#include <pbcopper/logging/LogConfig.h>

#include <optional>
#include <set>
#include <string>
#include <vector>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for main CLI interface
///
struct InterfaceData
{
    InterfaceData(std::string appName, std::string appDescription, std::string appVersion,
                  OptionData help, OptionData version, OptionData numThreads, OptionData logFile,
                  OptionData logLevel, OptionData alarms, OptionData exceptionPassthrough,
                  OptionData showAllHelp)
        : AppName{std::move(appName)}
        , AppDescription{std::move(appDescription)}
        , AppVersion{std::move(appVersion)}
        , HelpOption(std::move(help))        // icc 17 hack
        , VersionOption(std::move(version))  // icc 17 hack
        , NumThreadsOption{std::move(numThreads)}
        , LogFileOption{std::move(logFile)}
        , LogLevelOption{std::move(logLevel)}
        , AlarmsOption(std::move(alarms))                              // icc 17 hack
        , ExceptionPassthroughOption(std::move(exceptionPassthrough))  // icc 17 hack
        , ShowAllHelpOption(std::move(showAllHelp))                    // icc 17 hack
    {
        // default version printer
        VersionPrinter = [](const Interface& interface) {
            internal::VersionPrinter::Print(interface);
        };
    }

    std::string AppName;
    std::string AppDescription;
    std::string AppVersion;

    std::string Example;
    std::string Footer;

    // default enabled
    OptionData HelpOption;
    OptionData VersionOption;
    std::optional<internal::OptionData> NumThreadsOption;
    std::optional<internal::OptionData> LogFileOption;
    std::optional<internal::OptionData> LogLevelOption;

    // always active, but hidden
    internal::OptionData AlarmsOption;
    internal::OptionData ExceptionPassthroughOption;
    internal::OptionData ShowAllHelpOption;

    // default disabled
    std::optional<internal::OptionData> VerboseOption;

    Logging::LogConfig LogConfig;

    std::vector<OptionGroupData> OptionGroups;
    std::vector<PositionalArgumentData> PositionalArgs;
    std::set<std::string> AllOptionNames;

    VersionPrinterCallback VersionPrinter;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACEDATA_H
