// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_INTERFACEDATA_H
#define PBCOPPER_CLI_v2_INTERFACEDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>
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
                  OptionData logLevel)
        : appName_{std::move(appName)}
        , appDescription_{std::move(appDescription)}
        , appVersion_{std::move(appVersion)}
        , helpOption_{std::move(help)}
        , versionOption_{std::move(version)}
        , numThreadsOption_{std::move(numThreads)}
        , logFileOption_{std::move(logFile)}
        , logLevelOption_{std::move(logLevel)}
    {
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

    // default disabled
    boost::optional<internal::OptionData> verboseOption_ = boost::none;

    Logging::LogConfig logConfig_;

    std::vector<OptionGroupData> optionGroups_;
    std::vector<PositionalArgumentData> positionalArgs_;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACEDATA_H
