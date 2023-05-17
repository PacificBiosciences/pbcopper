#ifndef PBCOPPER_UTILITY_FASTMOD_H
#define PBCOPPER_UTILITY_FASTMOD_H

#include <pbcopper/PbcopperConfig.h>

#include <limits>
#include <utility>

#include <cassert>
#include <cstdint>

#ifndef INLINE
#define INLINE_UNDEFINE_AT_END
#if __GNUC__ || __clang__
#define INLINE __attribute__((always_inline)) inline
#elif __CUDACC__
#define INLINE __forceinline__ inline
#else
#define INLINE inline
#endif
#endif

namespace PacBio {
namespace Utility {

__uint128_t ComputeMultInverse64(std::uint64_t d) noexcept;
uint64_t FastDivide64(std::uint64_t a, __uint128_t M) noexcept;
uint64_t FastModulus64(std::uint64_t a, __uint128_t M, std::uint64_t d) noexcept;
uint64_t ComputeMultInverse32(std::uint32_t d) noexcept;

uint32_t FastModulus32(std::uint32_t a, std::uint64_t M, std::uint32_t d) noexcept;
uint32_t FastDivide32(std::uint32_t a, std::uint64_t M) noexcept;

// fastmod computes (a / d) given precomputed M for d>1

template <typename T>
struct DivisionResult
{
    T Quotient;
    T Remainder;

    T& first() { return this->Quotient; }

    T& first() const { return this->Quotient; }

    T& second() { return this->Remainder; }

    T& second() const { return this->Remainder; }

    std::pair<T, T> to_pair() const { return {Quotient, Remainder}; }
};

template <typename T, bool shortcircuit = false>
struct FastMod;

template <bool shortcircuit>
struct FastMod<std::uint64_t, shortcircuit>
{
private:
    std::uint64_t d_;
    __uint128_t M_;
    std::uint64_t m32_;
    std::uint64_t& m32() { return m32_; }
    // We swap location here so that m32 can be 64-bit aligned.
public:
    FastMod(std::uint64_t d) : d_(d), M_(ComputeMultInverse64(d))
    {
        if constexpr (shortcircuit) {
            m32_ = ComputeMultInverse32(d);
        } else {
            m32_ = 0;
        }
    }

    const std::uint64_t& d() const { return d_; }

    const std::uint64_t& m32() const { return m32_; }

    INLINE bool TestLimits(std::uint64_t v) const
    {
        constexpr std::uint64_t THRESHOLD = std::numeric_limits<std::uint32_t>::max();
        return (d_ <= THRESHOLD) && (v <= THRESHOLD);
    }

    INLINE std::uint64_t Divide(std::uint64_t v) const
    {
        if (shortcircuit) {
            return TestLimits(v) ? std::uint64_t(FastDivide32(v, m32_)) : FastDivide64(v, m32_);
        }
        return FastDivide64(v, M_);
    }

    INLINE std::uint64_t Modulus(std::uint64_t v) const
    {
        if (shortcircuit) {
            return TestLimits(v) ? std::uint64_t(FastModulus32(v, m32_, d_))
                                 : FastModulus64(v, m32_, d_);
        }
        return FastModulus64(v, M_, d_);
    }

    INLINE DivisionResult<std::uint64_t> DivMod(std::uint64_t v) const
    {
        const std::uint64_t d = Divide(v);
        return DivisionResult<std::uint64_t>{d, v - d_ * d};
    }
};

template <>
struct FastMod<std::uint32_t>
{
    const std::uint32_t d_;
    const std::uint64_t M_;

    FastMod(std::uint32_t d) : d_(d), M_(ComputeMultInverse32(d)) {}

    std::uint32_t d() const { return d_; }

    INLINE std::uint32_t Divide(std::uint32_t v) const { return FastDivide32(v, M_); }

    INLINE std::uint32_t Modulus(std::uint32_t v) const { return FastModulus32(v, M_, d_); }

    INLINE DivisionResult<std::uint32_t> DivMod(std::uint32_t v) const
    {
        const std::uint32_t tmpd = Divide(v);
        return DivisionResult<std::uint32_t>{tmpd, v - d_ * tmpd};
    }
};

}  // namespace Utility
}  // namespace PacBio

#ifdef INLINE_UNDEFINE_AT_END
#undef INLINE_UNDEFINE_AT_END
#undef INLINE
#endif

#endif /* PBCOPPER_UTILITY_FASTMOD_H  */
