// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_INTERFACEDATA_H
#define PBCOPPER_CLI_v2_INTERFACEDATA_H

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for main CLI interface
///
struct InterfaceData
{
    InterfaceData(std::string appName, std::string appDescription, std::string appVersion,
                  OptionData help, OptionData logFile, OptionData logLevel, OptionData numThreads,
                  OptionData version)
        : appName_{std::move(appName)}
        , appDescription_{std::move(appDescription)}
        , appVersion_{std::move(appVersion)}
        , helpOption_{std::move(help)}
        , logFileOption_{std::move(logFile)}
        , logLevelOption_{std::move(logLevel)}
        , numThreadsOption_{std::move(numThreads)}
        , versionOption_{std::move(version)}
    {
    }

    std::string appName_;
    std::string appDescription_;
    std::string appVersion_;

    std::string example_;
    std::string footer_;

    OptionData helpOption_;
    OptionData logFileOption_;
    OptionData logLevelOption_;
    OptionData numThreadsOption_;
    OptionData versionOption_;

    std::vector<OptionGroupData> optionGroups_;
    std::vector<PositionalArgumentData> positionalArgs_;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACEDATA_H
