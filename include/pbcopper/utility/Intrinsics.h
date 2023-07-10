#ifndef PBCOPPER_UTILITY_INTRINSICS_H
#define PBCOPPER_UTILITY_INTRINSICS_H

#include <pbcopper/PbcopperConfig.h>

#include <type_traits>
#include <version>

#include <cassert>
#include <climits>
#include <cstdint>

/*
 * Because C++-20 bit operations are not standard across compilers, this header
 * provides convenience wrappers allowing for consistant cross-platform behavior.
 * We currently wrap:
 *  1. PopCount (std::popcount)
 *  2. CountLeadingZeros (std::countl_zero)
 *  3. CountTrailingZeros (std::countr_zero)
 *  4. HasSingleBit (std::has_single_bit)
 *  5. BitCeiling (std::bit_ceil)
 */

#if defined(__cpp_lib_bitops)
#if __cpp_lib_bitops >= 201907L
#include <bit>
#define PB_BITOPS_ENABLED
#endif
#endif

namespace PacBio {
namespace Utility {

// PopCount

// * std::int8_t/uint8_t
// * std::int16_t/uint16_t
// * std::int32_t/uint32_t
template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr
    typename std::enable_if_t<std::is_integral_v<T> && (sizeof(T) <= 4), int>
    PopCount(const T x) noexcept
{
    // For std::int8_t and std::int16_t, we first have to convert to the unsigned type
    // of the same rank, and then cast to std::uint32_t, since we would
    // sign-extend x (increasing the number of set bits) to 32 bits otherwise.
    const unsigned int paddedX = std::make_unsigned_t<T>(x);

#ifdef __CUDA_ARCH__
    if (__builtin_is_constant_evaluated()) {
#endif
        // constexpr or host
        return
#ifdef PB_BITOPS_ENABLED
            // C++20
            std::popcount(paddedX)
#else
        // C++17 and below
        // TODO: remove once we're on C++20 in CUDA too
        __builtin_popcount(paddedX)
#endif  // PB_BITOPS_ENABLED
                ;
#ifdef __CUDA_ARCH__
    } else {
        // device
        return __popc(paddedX);
    }
#endif
}

// * std::int64_t/uint64_t
template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr
    typename std::enable_if_t<std::is_integral_v<T> && (sizeof(T) == 8), int>
    PopCount(const T x) noexcept
{
    const unsigned long long int val = x;

#ifdef __CUDA_ARCH__
    if (__builtin_is_constant_evaluated()) {
#endif
        // constexpr or host
        return
#ifdef PB_BITOPS_ENABLED
            // C++20
            std::popcount(val)
#else
        // C++17 and below
        // TODO: remove once we're on C++20 in CUDA too
        __builtin_popcountll(val)
#endif  // PB_BITOPS_ENABLED
                ;
#ifdef __CUDA_ARCH__
    } else {
        // device
        return __popcll(val);
    }
#endif
}

// * __int128_t/__uint128_t
template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr
    typename std::enable_if_t<std::is_same_v<T, __uint128_t> || std::is_same_v<T, __int128_t>, int>
    PopCount(const T x) noexcept
{
    const __uint128_t val = x;

    return PopCount(std::uint64_t(val)) + PopCount(std::uint64_t(val >> 64));
}

// Count Trailing Zeros
// no __CUDA_ARCH__ code paths, since CUDA has no __builtin_ctz equivalent

template <typename T>
int CountTrailingZeros(T x) noexcept
{
    if constexpr (sizeof(T) == 16) {
        const int trailingLow = CountTrailingZeros(std::uint64_t(x));
        if (trailingLow != static_cast<int>(sizeof(T) * CHAR_BIT)) {
            return trailingLow;
        }
        return CountTrailingZeros(std::uint64_t(x >> 64)) + 64;
    } else {
#ifdef PB_BITOPS_ENABLED
        return std::countr_zero(x);
#else
        if constexpr (std::is_same_v<std::make_signed_t<T>, long signed int>) {
            return __builtin_ctzl(x);
        } else if constexpr (std::is_same_v<std::make_signed_t<T>, long long signed int>) {
            return __builtin_ctzll(x);
        } else {
            return __builtin_ctz(static_cast<int>(x));
        }
#endif
    }
}

// Count Leading Zeros

// * std::int8_t/uint8_t
// * std::int16_t/uint16_t
// * std::int32_t/uint32_t
template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr
    typename std::enable_if_t<std::is_integral_v<T> && (sizeof(T) <= 4), int>
    CountLeadingZeros(const T x) noexcept
{
    // For std::int8_t and std::int16_t, we first have to convert to the unsigned type
    // of the same rank, and then cast to std::uint32_t, since we would
    // sign-extend x (increasing the number of set bits) to 32 bits otherwise.
    const auto unsignedX = std::make_unsigned_t<T>(x);
    [[maybe_unused]] const unsigned int paddedX = unsignedX;

    [[maybe_unused]] constexpr int NUM_PADDED_BITS = (4 - sizeof(T)) * CHAR_BIT;

#ifdef __CUDA_ARCH__
    if (__builtin_is_constant_evaluated()) {
#endif
        // constexpr or host
        return
#ifdef PB_BITOPS_ENABLED
            // C++20
            std::countl_zero(unsignedX)
#else
        // C++17 and below
        // TODO: remove once we're on C++20 in CUDA too
        // UB if the input is 0
        //   https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-_005f_005fbuiltin_005fclz
        (paddedX ? __builtin_clz(paddedX) : 32) - NUM_PADDED_BITS
#endif  // PB_BITOPS_ENABLED
                ;
#ifdef __CUDA_ARCH__
    } else {
        // device
        return __clz(paddedX) - NUM_PADDED_BITS;
    }
#endif
}

// * std::int64_t/uint64_t
template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr
    typename std::enable_if_t<std::is_integral_v<T> && (sizeof(T) == 8), int>
    CountLeadingZeros(const T x) noexcept
{
    const unsigned long long int val = x;

#ifdef __CUDA_ARCH__
    if (__builtin_is_constant_evaluated()) {
#endif
        // constexpr or host
        return
#ifdef PB_BITOPS_ENABLED
            // C++20
            std::countl_zero(val)
#else
        // C++17 and below
        // TODO: remove once we're on C++20 in CUDA too
        // UB if the input is 0
        //   https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-_005f_005fbuiltin_005fclzll
        val ? __builtin_clzll(val) : 64
#endif  // PB_BITOPS_ENABLED
                ;
#ifdef __CUDA_ARCH__
    } else {
        // device
        return __clzll(val);
    }
#endif
}

// * __int128_t/__uint128_t
template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr
    typename std::enable_if_t<std::is_same_v<T, __uint128_t> || std::is_same_v<T, __int128_t>, int>
    CountLeadingZeros(const T x) noexcept
{
    const __uint128_t val = x;

    const int leadingHi = CountLeadingZeros(std::uint64_t(val >> 64));
    if (leadingHi != static_cast<int>(sizeof(T) * CHAR_BIT)) {
        return leadingHi;
    }
    return CountLeadingZeros(std::uint64_t(val)) + 64;
}

// We could use std::has_single_bit, but this is simpler
// and has_single_bit implementations are usually based on popcount
template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr bool HasSingleBit(const T x) noexcept
{
    return PopCount(x) == 1;
}

template <typename T>
T BitCeiling(T x) noexcept
{
#ifdef PB_BITOPS_ENABLED
    if constexpr (sizeof(T) <= 8) {
        return std::bit_ceil(static_cast<std::make_unsigned_t<T>>(x));
    }
#endif
    if (!x) {
        return static_cast<T>(1);
    }
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    if constexpr (sizeof(x) > 1) {
        x |= x >> 8;
        if constexpr (sizeof(x) > 2) {
            x |= x >> 16;
            if constexpr (sizeof(x) > 4) {
                x |= x >> 32;
                if constexpr (sizeof(x) > 8) {
                    x |= x >> 64;
                }
            }
        }
    }
    return ++x;
}

template <typename T>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr int IntegralLog2(const T x)
{
    // UB if x <= 0
    assert(x > 0);
    return sizeof(T) * CHAR_BIT - Utility::CountLeadingZeros(x) - 1;
}

}  // namespace Utility
}  // namespace PacBio

#undef PB_BITOPS_ENABLED

#endif /* PBCOPPER_UTILITY_INTRINSICS_H */
