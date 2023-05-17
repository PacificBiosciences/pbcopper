#ifndef PBCOPPER_PBMER_KMER_H
#define PBCOPPER_PBMER_KMER_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Strand.h>

#include <string>

#include <cstdint>

namespace PacBio {
namespace Pbmer {

class Kmer
{
public:
    // 32 DNA bases are packed into a uint64
    std::uint64_t mer = 0;

    // position of the kmer
    std::uint32_t pos = 1;

    // strand of the kmer
    Data::Strand strand = Data::Strand::FORWARD;

    Kmer();
    Kmer(Data::Strand s);
    Kmer(std::uint64_t k, std::uint32_t p, Data::Strand s);

    bool operator==(Kmer const& b) const noexcept;
    bool operator!=(Kmer const& b) const noexcept;

    // hashes both kmers then return a bool evaluating a <= b
    void ReverseComp(std::uint8_t kmerSize);
    bool LexSmaller(Kmer const& b, std::uint8_t kmerSize) const;
    std::string KmerToStr(std::uint8_t kmerSize) const;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_KMER_H
