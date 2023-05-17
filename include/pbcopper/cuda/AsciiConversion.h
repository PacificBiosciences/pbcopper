#ifndef PBCOPPER_CUDA_ASCIICONVERSION_H
#define PBCOPPER_CUDA_ASCIICONVERSION_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/container/BitConversion.h>
#include <pbcopper/container/BitmaskContainer.h>

#include <cstdint>

namespace PacBio {
namespace Cuda {

template <std::int32_t TotalBits, std::int32_t ElementBits, std::int32_t N>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr Container::BitmaskContainer<TotalBits, ElementBits>
AsciiToBitmaskContainer(const std::uint32_t (&in)[N], const std::int32_t len)
{
    Container::BitmaskContainer<TotalBits, ElementBits> result;
    static_assert(result.Capacity() >= (N * 4),
                  "Return type does not have sufficient capacity for provided ASCII bases");

    Container::BitmaskContainer<32, 8> buf;
    for (std::int32_t i = 0; i < (N * 4); ++i) {
        if ((i % 4) == 0) {
            buf = Container::BitmaskContainer<32, 8>{in[i / 4]};
        }

        const char base = buf[i % 4];
        const std::int32_t encodedBase{(i < len) ? Container::ConvertAsciiTo2bit(base) : 0};
        result.Set(i, encodedBase);
    }

    return result;
}

template <std::int32_t TotalBits, std::int32_t ElementBits, std::int32_t N>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr void BitmaskContainerToAscii(
    const Container::BitmaskContainer<TotalBits, ElementBits> in, const std::int32_t len,
    std::uint32_t (&out)[N])
{
    static_assert((N * 4) >= in.Capacity(),
                  "Out array does not have sufficient capacity for provided compressed string");

    Container::BitmaskContainer<32, 8> buf;
    for (std::int32_t i = 0; i < (N * 4); ++i) {
        if ((i % 4) == 0) {
            buf.Clear();
        }

        const char base = Container::Convert2bitToAscii(in[i]);
        buf.Set(i % 4, (i < len) ? base : 0);

        if ((i + 1) % 4 == 0) {
            out[i / 4] = buf.RawData();
        }
    }
}

}  // namespace Cuda
}  // namespace PacBio

#endif  // PBCOPPER_CUDA_ASCIICONVERSION_H
