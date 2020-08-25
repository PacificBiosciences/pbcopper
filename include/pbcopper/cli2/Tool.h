// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_TOOL_H
#define PBCOPPER_CLI_v2_TOOL_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/ResultsHandler.h>

namespace PacBio {
namespace CLI_v2 {

enum class ToolVisibility
{
    VISIBLE,
    HIDDEN
};

struct Tool
{
    std::string name;
    Interface interface;
    ResultsHandler runner;
    ToolVisibility visibility = ToolVisibility::VISIBLE;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_TOOL_H
