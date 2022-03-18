#include <pbcopper/utility/FastMod.h>

namespace PacBio {
namespace Utility {

__uint128_t ComputeMultInverse64(uint64_t d) noexcept { return (__uint128_t(-1) / d) + 1; }

uint64_t Mul128Hi(__uint128_t lowbits, uint64_t d) noexcept
{
    __uint128_t bottomHalf = (lowbits & UINT64_C(0xFFFFFFFFFFFFFFFF)) * d;  // Won't overflow
    bottomHalf >>= 64;  // Only need the top 64 bits, as we'll shift the lower half away;
    __uint128_t topHalf = (lowbits >> 64) * d;
    __uint128_t bothHalves = bottomHalf + topHalf;  // Both halves are already shifted down by 64
    return bothHalves >> 64;                        // Get top half of bothHalves
}

uint64_t FastDivide64(uint64_t a, __uint128_t multiplicativeInverse) noexcept
{
    return Mul128Hi(multiplicativeInverse, a);
}

uint64_t FastModulus64(uint64_t a, __uint128_t multiplicativeInverse, uint64_t d) noexcept
{
    return Mul128Hi(multiplicativeInverse * a, d);
}

uint64_t ComputeMultInverse32(uint32_t d) noexcept { return UINT64_C(0xFFFFFFFFFFFFFFFF) / d + 1; }

uint64_t Mul64By32(uint64_t lowbits, uint32_t d) noexcept
{
    return (static_cast<__uint128_t>(lowbits) * d) >> 64;
}

uint32_t FastModulus32(uint32_t a, uint64_t multiplicativeInverse, uint32_t d) noexcept
{
    uint64_t lowbits = multiplicativeInverse * a;
    return Mul64By32(lowbits, d);
}

uint32_t FastDivide32(uint32_t a, uint64_t multiplicativeInverse) noexcept
{
    return Mul64By32(multiplicativeInverse, a);
}

}  // namespace Utility
}  // namespace PacBio
