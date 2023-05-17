#ifndef PBCOPPER_CUDA_CUDA_H
#define PBCOPPER_CUDA_CUDA_H

#include <cuda_runtime_api.h>

#include <cassert>
#include <cstddef>
#include <stdexcept>

namespace PacBio {
namespace Cuda {

//
// Convenience allocators/deleters intended for use with GPU code
//

// Host memory
struct PinnedWriteCombinedMemoryAllocator
{
    std::byte* operator()(const std::ptrdiff_t size) const
    {
        assert(size >= 0);

        std::byte* ptr = nullptr;
        const auto status =
            cudaHostAlloc(reinterpret_cast<void**>(&ptr), size, cudaHostAllocWriteCombined);
        if (status != cudaSuccess) {
            throw std::runtime_error(
                "[pbcopper] ERROR: error allocating write-combined pinned memory");
        }

        return ptr;
    }
};

struct PinnedMemoryAllocator
{
    std::byte* operator()(const std::ptrdiff_t size) const
    {
        assert(size >= 0);

        std::byte* ptr = nullptr;
        const auto status = cudaMallocHost(reinterpret_cast<void**>(&ptr), size);
        if (status != cudaSuccess) {
            throw std::runtime_error("[pbcopper] ERROR: error allocating pinned memory");
        }

        return ptr;
    }
};

struct PinnedMemoryDeleter
{
    void operator()(void* ptr) const noexcept
    {
        // counter-part to cudaMallocHost on the CPU
        cudaFreeHost(ptr);
    }
};

// Device memory
struct DeviceMemoryAllocator
{
    std::byte* operator()(const std::ptrdiff_t size) const
    {
        assert(size >= 0);

        std::byte* ptr = nullptr;
        const auto status = cudaMalloc(reinterpret_cast<void**>(&ptr), size);
        if (status != cudaSuccess) {
            throw std::runtime_error("[pbcopper] ERROR: error allocating device memory");
        }

        return ptr;
    }
};

struct GPUMemoryDeleter
{
    void operator()(void* ptr) const noexcept
    {
        // counter-part to cudaMalloc on the GPU
        cudaFree(ptr);
    }
};

}  // namespace Cuda
}  // namespace PacBio

#endif  // PBCOPPER_CUDA_CUDA_H
