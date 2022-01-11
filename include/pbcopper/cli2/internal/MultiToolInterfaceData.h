#ifndef PBCOPPER_CLI_v2_MULTITOOLINTERFACEDATA_H
#define PBCOPPER_CLI_v2_MULTITOOLINTERFACEDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/Tool.h>
#include <pbcopper/cli2/VersionPrinterCallback.h>
#include <pbcopper/cli2/internal/InterfaceData.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/VersionPrinter.h>
#include <pbcopper/logging/LogConfig.h>

#include <memory>
#include <optional>
#include <string>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for multi-tool CLI
///
struct MultiToolInterfaceData
{
    MultiToolInterfaceData(std::string appName, std::string appDescription, std::string appVersion,
                           OptionData help, OptionData versionOption, OptionData showAllHelpOption)
        : appName_{std::move(appName)}
        , appDescription_{std::move(appDescription)}
        , appVersion_{std::move(appVersion)}
        , helpOption_(std::move(help))                      // icc 17 hack
        , versionOption_(std::move(versionOption))          // icc 17 hack
        , showAllHelpOption_(std::move(showAllHelpOption))  // icc 17 hack
    {
        // default version printer
        versionPrinter_ = [](const MultiToolInterface& i) { internal::VersionPrinter::Print(i); };
    }

    std::string appName_;
    std::string appDescription_;
    std::string appVersion_;
    std::string helpFooter_;

    OptionData helpOption_;
    OptionData versionOption_;
    OptionData showAllHelpOption_;

    std::vector<Tool> tools_;

    std::optional<Logging::LogConfig> logConfig_;

    MultiToolVersionPrinterCallback versionPrinter_;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_MULTITOOLINTERFACEDATA_H
