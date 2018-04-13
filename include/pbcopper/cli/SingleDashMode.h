// Author: Derek Barnett

#ifndef PBCOPPER_CLI_SINGLEDASHMODE_H
#define PBCOPPER_CLI_SINGLEDASHMODE_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace CLI {

///
/// \brief The SingleDashMode enum
///
enum class SingleDashMode
{
    ParseAsShortOptions,  // "-abc" equivalent to "-a -b -c" (default & recommended)
    ParseAsLongOptions    // "-abc" equivalent to "--abc"
};

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_SINGLEDASHMODE_H
