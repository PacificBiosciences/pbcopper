#ifndef PBCOPPER_UTILITY_FASTMOD_H
#define PBCOPPER_UTILITY_FASTMOD_H

#include <pbcopper/PbcopperConfig.h>

#include <limits>
#include <utility>

#include <cassert>
#include <cstdint>

#ifndef INLINE
#define INLINE_UNDEFINE___
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

__uint128_t ComputeMultInverse64(uint64_t d) noexcept;
uint64_t FastDivide64(uint64_t a, __uint128_t M) noexcept;
uint64_t FastModulus64(uint64_t a, __uint128_t M, uint64_t d) noexcept;
uint64_t ComputeMultInverse32(uint32_t d) noexcept;

uint32_t FastModulus32(uint32_t a, uint64_t M, uint32_t d) noexcept;
uint32_t FastDivide32(uint32_t a, uint64_t M) noexcept;

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

    std::pair<T, T> to_pair() const { return std::make_pair(Quotient, Remainder); }
};

template <typename T, bool shortcircuit = false>
struct FastMod;

template <bool shortcircuit>
struct FastMod<uint64_t, shortcircuit>
{
private:
    uint64_t d_;
    __uint128_t M_;
    uint64_t m32_;
    uint64_t& m32() { return m32_; }
    // We swap location here so that m32 can be 64-bit aligned.
public:
    FastMod(uint64_t d) : d_(d), M_(ComputeMultInverse64(d))
    {
        if constexpr (shortcircuit) {
            m32_ = ComputeMultInverse32(d);
        } else {
            m32_ = 0;
        }
    }

    const uint64_t& d() const { return d_; }

    const uint64_t& m32() const { return m32_; }

    INLINE bool TestLimits(uint64_t v) const
    {
        static constexpr uint64_t threshold = std::numeric_limits<uint32_t>::max();
        return d_ <= threshold && v <= threshold;
    }

    INLINE uint64_t Divide(uint64_t v) const
    {
        if (shortcircuit) {
            return TestLimits(v) ? uint64_t(FastDivide32(v, m32_)) : FastDivide64(v, m32_);
        }
        return FastDivide64(v, M_);
    }

    INLINE uint64_t Modulus(uint64_t v) const
    {
        if (shortcircuit) {
            return TestLimits(v) ? uint64_t(FastModulus32(v, m32_, d_))
                                 : FastModulus64(v, m32_, d_);
        }
        return FastModulus64(v, M_, d_);
    }

    INLINE DivisionResult<uint64_t> DivMod(uint64_t v) const
    {
        const uint64_t d = Divide(v);
        return DivisionResult<uint64_t>{d, v - d_ * d};
    }
};

template <>
struct FastMod<uint32_t>
{
    const uint32_t d_;
    const uint64_t M_;

    FastMod(uint32_t d) : d_(d), M_(ComputeMultInverse32(d)) {}

    uint32_t d() const { return d_; }

    INLINE uint32_t Divide(uint32_t v) const { return FastDivide32(v, M_); }

    INLINE uint32_t Modulus(uint32_t v) const { return FastModulus32(v, M_, d_); }

    INLINE DivisionResult<uint32_t> DivMod(uint32_t v) const
    {
        const uint32_t tmpd = Divide(v);
        return DivisionResult<uint32_t>{tmpd, v - d_ * tmpd};
    }
};

}  // namespace Utility
}  // namespace PacBio

#ifdef INLINE_UNDEFINE___
#undef INLINE_UNDEFINE___
#undef INLINE
#endif

#endif /* PBCOPPER_UTILITY_FASTMOD_H  */
