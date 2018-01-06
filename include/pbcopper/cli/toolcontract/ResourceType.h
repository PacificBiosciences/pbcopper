#ifndef PBCOPPER_CLI_TOOLCONTRACT_RESOURCETYPE_H
#define PBCOPPER_CLI_TOOLCONTRACT_RESOURCETYPE_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

enum class ResourceType
{
    TMP_DIR
  , TMP_FILE
  , LOG_FILE
  , OUTPUT_DIR
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_TOOLCONTRACT_RESOURCETYPE_H
