#ifndef PBCOPPER_NUMERIC_HELPER_H
#define PBCOPPER_NUMERIC_HELPER_H

#include <pbcopper/PbcopperConfig.h>

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace PacBio {
namespace Numeric {

PB_CUDA_HOST PB_CUDA_DEVICE constexpr bool IsPowerOfTwo(const std::uint32_t x) noexcept
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

constexpr std::uint32_t NextPowerOfTwo(const std::uint32_t x) noexcept
{
    assert(x > 1);
    return 1U << (32 - __builtin_clz(x - 1));
}

template <std::uint32_t multiple, typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr T RoundUpDivision(const T x) noexcept
{
    static_assert(multiple > 0, "multiple is 0");
    static_assert(std::is_integral_v<T>, "T is not an integral type");

    // perform all divisions in unsigned space, in order to yield bitshift instructions
    constexpr std::make_unsigned_t<std::common_type_t<T, decltype(multiple)>> MULTIPLE = multiple;

    return (x + (MULTIPLE - 1U)) / MULTIPLE;
}

template <std::uint32_t multiple, typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr T RoundDownDivision(const T x) noexcept
{
    static_assert(multiple > 0, "multiple is 0");
    static_assert(std::is_integral_v<T>, "T is not an integral type");

    // perform all divisions in unsigned space, in order to yield bitshift instructions
    constexpr std::make_unsigned_t<std::common_type_t<T, decltype(multiple)>> MULTIPLE = multiple;

    return x / MULTIPLE;
}

template <std::uint32_t multiple, typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr T RoundUpToNextMultiple(const T x) noexcept
{
    static_assert(std::is_integral_v<T>, "T is not an integral type");

    constexpr std::make_unsigned_t<std::common_type_t<T, decltype(multiple)>> MULTIPLE = multiple;

    return RoundUpDivision<multiple>(x) * MULTIPLE;
}

template <std::uint32_t multiple, typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr T RoundDownToNextMultiple(const T x) noexcept
{
    static_assert(std::is_integral_v<T>, "T is not an integral type");

    constexpr std::make_unsigned_t<std::common_type_t<T, decltype(multiple)>> MULTIPLE = multiple;

    return RoundDownDivision<multiple>(x) * MULTIPLE;
}

}  // namespace Numeric
}  // namespace PacBio

#endif  // PBCOPPER_NUMERIC_HELPER_H
