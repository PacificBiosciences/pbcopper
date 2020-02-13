// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBMER_DNABIT_H
#define PBMER_DNABIT_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>
#include <string>

#include <pbcopper/data/Strand.h>

namespace PacBio {
namespace Pbmer {

// This should remain a function, it has a lot of general utilit
uint64_t ReverseComp64(const uint64_t mer, uint8_t kmerSize);
// This should remain a function, it has a lot of general utility.
uint64_t Mix64Masked(uint64_t key, uint8_t kmerSize);

class DnaBit
{
public:
    // 32 DNA bases are packed into a uint64
    uint64_t mer{0};
    // Strand and size (in bp; bits = 2*bp).
    // Keeping the mer size in the class makes life a lot easier.
    // 0:+ forward strand ; 1:- reverse strand
    uint8_t strand{0};
    uint8_t msize{0};

    DnaBit() = default;
    DnaBit(uint64_t k, uint8_t strand, uint8_t size);
    // Ignores strand, compares hashed kmer.
    bool operator>(DnaBit const& b) const;
    // Ignores strand, compares hashed kmer.
    bool operator<(DnaBit const& b) const;
    // Ignores strand, compares hashed kmer.
    bool operator>=(DnaBit const& b) const;
    // Ignores strand, compares hashed kmer.
    bool operator<=(DnaBit const& b) const;
    // Checks strand, not hashed kmer.
    bool operator==(DnaBit const& b) const;
    // Checks strand, not hashed kmer.
    bool operator!=(DnaBit const& b) const;
    // Returns the hashed smaller kmer (forward/reverse).
    DnaBit LexSmallerEq() const;
    // Returns the kmer string;
    std::string KmerToStr() const;
    // Reverse complements the kmer in place.
    void ReverseComp();
    // Put a base at the end of the kmer
    bool AppendBase(const char c);
    // Put a base at the beginning of kmer
    bool PrependBase(const char c);
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBMER_DNABIT_H
