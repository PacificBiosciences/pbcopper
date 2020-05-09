// Author: Lance Hepler, Derek Barnett

#ifndef PBCOPPER_DATA_SNR_H
#define PBCOPPER_DATA_SNR_H

#include <pbcopper/PbcopperConfig.h>

#include <ostream>
#include <vector>

namespace PacBio {
namespace Data {

// From UNY/include/pacbio/data/Read.h
// TODO: remove duplication there (and add to BamRecord?)

/// Stores nucleotide-wise signal to noise ratios.
struct SNR
{
    float A;
    float C;
    float G;
    float T;

    SNR(float a, float c, float g, float t) noexcept;
    SNR(const std::vector<float>& snrs) noexcept;
    SNR(const float (&snrs)[4]) noexcept;

    operator std::vector<float>() const;

    const float& operator[](const int i) const noexcept;
    float& operator[](const int i) noexcept;

    bool operator==(const SNR& other) const noexcept;
    bool operator!=(const SNR& other) const noexcept;

    float Minimum() const noexcept;
};

SNR ClampSNR(const SNR& val, const SNR& min, const SNR& max) noexcept;

std::ostream& operator<<(std::ostream& os, const SNR& snr);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_SNR_H
