// Author: Lance Hepler, Derek Barnett

#ifndef PBCOPPER_DATA_SNR_H
#define PBCOPPER_DATA_SNR_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>
#include <ostream>
#include <vector>

namespace PacBio {
namespace Data {

// From UNY/include/pacbio/data/Read.h
// TODO: remove duplication there (and add to BamRecord?)

/// Stores nucleotide-wise signal to noise ratios.
struct SNR
{
    double A;
    double C;
    double G;
    double T;

    SNR(double a, double c, double g, double t);
    SNR(const std::vector<double>& snrs);
    SNR(const std::vector<float>& snrs);
    SNR(const double (&snrs)[4]);

    operator std::vector<float>() const;

    const double& operator[](const size_t i) const;
    double& operator[](const size_t i);

    bool operator==(const SNR& other) const;
    bool operator!=(const SNR& other) const;

    double Minimum() const;
};

SNR ClampSNR(const SNR& val, const SNR& min, const SNR& max);

std::ostream& operator<<(std::ostream& os, const SNR& snr);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_SNR_H
