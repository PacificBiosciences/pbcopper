// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#include <array>
#include <cassert>
#include <tuple>

#include <pbcopper/pbmer/Kmer.h>
#include <pbcopper/pbmer/Mers.h>

namespace PacBio {
namespace Pbmer {

Kmer::Kmer(Data::Strand s) : strand{std::move(s)} {}

Kmer::Kmer(uint64_t k, uint32_t p, Data::Strand s) : mer{k}, pos{p}, strand{std::move(s)} {}

void Kmer::ReverseComp(uint8_t kmerSize)
{
    const uint64_t tmp = ~mer;
    uint64_t rv = 0;

    for (size_t i = 0; i < kmerSize; i++) {
        rv <<= 2;
        rv |= (tmp >> (i * 2) & 3);
    }

    mer = rv;
    //assert(strand != Data::Strand::UNMAPPED);
    if (strand == Data::Strand::FORWARD) {
        strand = Data::Strand::REVERSE;
    } else {
        strand = Data::Strand::FORWARD;
    }
}

std::string Kmer::KmerToStr(uint8_t kmerSize) const
{
    std::string bases;
    bases.resize(kmerSize);

    constexpr const std::array<char, 4> lookup{'A', 'C', 'G', 'T'};
    uint8_t i = 0;
    uint64_t tmp = 0;
    uint64_t offset = 0;

    while (i < kmerSize) {
        tmp = mer;
        offset = (kmerSize - i - 1) * 2;
        tmp >>= offset;
        bases[i] = lookup[3ull & tmp];
        ++i;
    }

    return bases;
}

bool Kmer::operator==(const Kmer& b) const
{
    return std::tie(mer, pos, strand) == std::tie(b.mer, b.pos, b.strand);
}

bool Kmer::operator!=(const Kmer& b) const { return !(*this == b); }

bool Kmer::LexSmaller(const Kmer& b, uint8_t kmerSize) const
{
    const uint64_t mask = (1ull << 2 * kmerSize) - 1;
    return Mers::Mix64Masked(mer, mask) <= Mers::Mix64Masked(b.mer, mask);
}

}  // namespace Pbmer
}  // namespace PacBio
