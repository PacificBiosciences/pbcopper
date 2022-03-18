#ifndef PBCOPPER_UTILITY_INTRINSICS_H
#define PBCOPPER_UTILITY_INTRINSICS_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>
#include <type_traits>

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

template <typename T>
int PopCount(T x) noexcept
{
    static_assert(
        std::is_integral_v<T> || std::is_same_v<T, __uint128_t> || std::is_same_v<T, __int128_t>,
        "Must be integral, U128 or I128");
    if constexpr (sizeof(T) == 16) {
        return PopCount(static_cast<uint64_t>(x)) + PopCount(static_cast<uint64_t>(x >> 64));
    } else {
#ifdef PB_BITOPS_ENABLED
        if constexpr (std::is_integral_v<T>) {
            return std::popcount(static_cast<std::make_unsigned_t<T>>(x));
        } else {
            return PopCount(uint64_t(x)) + PopCount(uint64_t(x >> 64));
        }
#else
        if constexpr (std::is_same_v<std::make_signed_t<T>, long signed int>) {
            return __builtin_popcountl(x);
        } else if constexpr (std::is_same_v<std::make_signed_t<T>, long long signed int>) {
            return __builtin_popcountll(x);
        } else {
            return __builtin_popcount(static_cast<int>(x));
        }
#endif
    }
}

// Count Trailing Zeros

template <typename T>
int CountTrailingZeros(T x) noexcept
{
    if constexpr (sizeof(T) == 16) {
        const int trailingLow = CountTrailingZeros(uint64_t(x));
        if (trailingLow != static_cast<int>(sizeof(T) * 8)) {
            return trailingLow;
        }
        return CountTrailingZeros(uint64_t(x >> 64)) + 64;
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

template <typename T>
int CountLeadingZeros(T x) noexcept
{
    if constexpr (sizeof(T) == 16) {
        const int leadingHi = CountLeadingZeros(uint64_t(x >> 64));
        if (leadingHi != static_cast<int>(sizeof(T) * 8)) {
            return leadingHi;
        }
        return CountLeadingZeros(uint64_t(x)) + 64;
    } else {
#ifdef PB_BITOPS_ENABLED
        return std::countl_zero(x);
#else
        if constexpr (std::is_same_v<std::make_signed_t<T>, long signed int>) {
            return __builtin_clzl(x);
        } else if constexpr (std::is_same_v<std::make_signed_t<T>, long long signed int>) {
            return __builtin_clzll(x);
        } else {
            return __builtin_clz(static_cast<int>(x));
        }
#endif
    }
}

// We could use std::has_single_bit, but this is simpler
// and has_single_bit implementations are usually based on popcount
template <typename T>
bool HasSingleBit(T x) noexcept
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
// Only correct if x > 0, but fast
template <typename T>
int IntegralLog2(T x)
{
    return sizeof(T) * 8 - Utility::CountLeadingZeros(x) - 1;
}

}  // namespace Utility
}  // namespace PacBio

#undef PB_BITOPS_ENABLED

#endif /* PBCOPPER_UTILITY_INTRINSICS_H */
