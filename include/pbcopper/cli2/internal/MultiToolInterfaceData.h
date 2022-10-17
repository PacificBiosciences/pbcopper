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
        : AppName{std::move(appName)}
        , AppDescription{std::move(appDescription)}
        , AppVersion{std::move(appVersion)}
        , HelpOption(std::move(help))                      // icc 17 hack
        , VersionOption(std::move(versionOption))          // icc 17 hack
        , ShowAllHelpOption(std::move(showAllHelpOption))  // icc 17 hack
    {
        // default version printer
        VersionPrinter = [](const MultiToolInterface& i) { internal::VersionPrinter::Print(i); };
    }

    std::string AppName;
    std::string AppDescription;
    std::string AppVersion;
    std::string HelpFooter;

    OptionData HelpOption;
    OptionData VersionOption;
    OptionData ShowAllHelpOption;

    std::vector<Tool> Tools;

    std::optional<Logging::LogConfig> LogConfig;

    MultiToolVersionPrinterCallback VersionPrinter;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_MULTITOOLINTERFACEDATA_H
