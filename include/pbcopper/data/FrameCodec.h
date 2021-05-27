#ifndef PBCOPPER_DATA_FRAMECODEC_H
#define PBCOPPER_DATA_FRAMECODEC_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Data {

/// \brief This enum describes the encoding types used for frame data.
///
enum class FrameCodec
{
    RAW,
    V1,
    V2,
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_FRAMECODEC_H
