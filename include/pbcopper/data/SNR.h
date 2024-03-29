#ifndef PBCOPPER_DATA_SNR_H
#define PBCOPPER_DATA_SNR_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <iosfwd>
#include <iterator>
#include <vector>

#include <cassert>
#include <cstdint>

namespace PacBio {
namespace Data {

// From UNY/include/pacbio/data/Read.h
// TODO: remove duplication there (and add to BamRecord?)

/// Stores nucleotide-wise signal to noise ratios.
class alignas(16) SNR
{
public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr SNR(const float a, const float c, const float g,
                                              const float t) noexcept
        : data_{a, c, g, t}
    {}

    SNR(const std::vector<float>& snrs) noexcept : SNR{snrs[0], snrs[1], snrs[2], snrs[3]}
    {
        assert(snrs.size() == 4);
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr SNR(const float (&snrs)[4]) noexcept
        : SNR{snrs[0], snrs[1], snrs[2], snrs[3]}
    {}

public:
    operator std::vector<float>() const;

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr const float& operator[](
        const std::int32_t i) const noexcept
    {
        assert(i >= 0);
        assert(i < 4);
        return data_[i];
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr float& operator[](const std::int32_t i) noexcept
    {
        // casting away const when underlying object is non-const, is well-defined
        // Effective C++ (Third Edition) - Item 3
        return const_cast<float&>(static_cast<const SNR&>(*this)[i]);
    }

public:
    constexpr float Minimum() const noexcept
    {
        return *std::min_element(std::cbegin(data_), std::cend(data_));
    }

    constexpr float Maximum() const noexcept
    {
        return *std::max_element(std::cbegin(data_), std::cend(data_));
    }

public:
    constexpr bool operator==(const SNR& rhs) const noexcept
    {
        return (data_[0] == rhs.data_[0]) && (data_[1] == rhs.data_[1]) &&
               (data_[2] == rhs.data_[2]) && (data_[3] == rhs.data_[3]);
    }

private:
    float data_[4];
};

constexpr bool operator!=(const SNR& lhs, const SNR& rhs) noexcept { return !(lhs == rhs); }

PB_CUDA_HOST PB_CUDA_DEVICE constexpr SNR ClampSNR(SNR v, const SNR& lo, const SNR& hi) noexcept
{
#ifndef __CUDA_ARCH__  // host
    using std::max;
    using std::min;
#endif

    for (std::int32_t i = 0; i < 4; ++i) {
        const float lowerBound = lo[i];
        const float upperBound = hi[i];
        assert(lowerBound < upperBound);

        v[i] = max(lowerBound, min(upperBound, v[i]));
    }
    return v;
}

std::ostream& operator<<(std::ostream& os, const SNR& snr);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_SNR_H
