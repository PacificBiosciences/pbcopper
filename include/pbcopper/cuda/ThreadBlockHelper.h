#ifndef PBCOPPER_CUDA_THREADBLOCKHELPER_H
#define PBCOPPER_CUDA_THREADBLOCKHELPER_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Cuda {

PB_CUDA_HOST PB_CUDA_DEVICE constexpr auto TotalThreadsBlock(const dim3 block) noexcept
{
    return block.x * block.y * block.z;
}

}  // namespace Cuda
}  // namespace PacBio

#endif  // PBCOPPER_CUDA_THREADBLOCKHELPER_H
