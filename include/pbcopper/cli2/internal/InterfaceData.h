#ifndef PBCOPPER_CLI_v2_INTERFACEDATA_H
#define PBCOPPER_CLI_v2_INTERFACEDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/VersionPrinterCallback.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>
#include <pbcopper/cli2/internal/VersionPrinter.h>
#include <pbcopper/logging/LogConfig.h>

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
                  OptionData logLevel, OptionData alarms, OptionData exceptionPassthrough)
        : appName_{std::move(appName)}
        , appDescription_{std::move(appDescription)}
        , appVersion_{std::move(appVersion)}
        , helpOption_(std::move(help))        // icc 17 hack
        , versionOption_(std::move(version))  // icc 17 hack
        , numThreadsOption_{std::move(numThreads)}
        , logFileOption_{std::move(logFile)}
        , logLevelOption_{std::move(logLevel)}
        , alarmsOption_{std::move(alarms)}
        , exceptionPassthroughOption_(std::move(exceptionPassthrough))  // icc 17 hack
    {
        // default version printer
        versionPrinter_ = [](const Interface& interface) {
            internal::VersionPrinter::Print(interface);
        };
    }

    std::string appName_;
    std::string appDescription_;
    std::string appVersion_;

    std::string example_;
    std::string footer_;

    // default enabled
    OptionData helpOption_;
    OptionData versionOption_;
    boost::optional<internal::OptionData> numThreadsOption_;
    boost::optional<internal::OptionData> logFileOption_;
    boost::optional<internal::OptionData> logLevelOption_;
    boost::optional<internal::OptionData> alarmsOption_;

    // always active
    internal::OptionData exceptionPassthroughOption_;

    // default disabled
    boost::optional<internal::OptionData> verboseOption_;

    Logging::LogConfig logConfig_;

    std::vector<OptionGroupData> optionGroups_;
    std::vector<PositionalArgumentData> positionalArgs_;

    VersionPrinterCallback versionPrinter_;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACEDATA_H
