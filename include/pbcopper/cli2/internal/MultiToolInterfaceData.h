// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_MULTITOOLINTERFACEDATA_H
#define PBCOPPER_CLI_v2_MULTITOOLINTERFACEDATA_H

#include <memory>
#include <string>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/Tool.h>
#include <pbcopper/cli2/internal/InterfaceData.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for multi-tool CLI
///
struct MultiToolInterfaceData
{
    MultiToolInterfaceData(std::string appName, std::string appDescription, std::string appVersion)
        : appName_{std::move(appName)}
        , appDescription_{std::move(appDescription)}
        , appVersion_{std::move(appVersion)}
    {
    }

    std::string appName_;
    std::string appDescription_;
    std::string appVersion_;
    std::string helpFooter_;

    std::vector<Tool> tools_;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_MULTITOOLINTERFACEDATA_H
