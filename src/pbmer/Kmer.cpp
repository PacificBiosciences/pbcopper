#include <pbcopper/pbmer/Kmer.h>

#include <pbcopper/pbmer/Mers.h>

#include <array>
#include <tuple>

#include <cassert>

namespace PacBio {
namespace Pbmer {

Kmer::Kmer() = default;

Kmer::Kmer(Data::Strand s) : strand{std::move(s)} {}

Kmer::Kmer(std::uint64_t k, std::uint32_t p, Data::Strand s) : mer{k}, pos{p}, strand{std::move(s)}
{}

bool Kmer::operator==(const Kmer& b) const noexcept
{
    return std::tie(mer, pos, strand) == std::tie(b.mer, b.pos, b.strand);
}

bool Kmer::operator!=(const Kmer& b) const noexcept { return !(*this == b); }

std::string Kmer::KmerToStr(std::uint8_t kmerSize) const
{
    std::string bases;
    bases.resize(kmerSize);

    constexpr std::array<char, 4> LOOKUP_TABLE{'A', 'C', 'G', 'T'};
    std::uint8_t i = 0;
    std::uint64_t tmp = 0;
    std::uint64_t offset = 0;

    while (i < kmerSize) {
        tmp = mer;
        offset = (kmerSize - i - 1) * 2;
        tmp >>= offset;
        bases[i] = LOOKUP_TABLE[3ull & tmp];
        ++i;
    }

    return bases;
}

bool Kmer::LexSmaller(const Kmer& b, std::uint8_t kmerSize) const
{
    const std::uint64_t mask = (1ull << 2 * kmerSize) - 1;
    return Mers::Mix64Masked(mer, mask) <= Mers::Mix64Masked(b.mer, mask);
}

void Kmer::ReverseComp(std::uint8_t kmerSize)
{
    const std::uint64_t tmp = ~mer;
    std::uint64_t rv = 0;

    for (std::size_t i = 0; i < kmerSize; i++) {
        rv <<= 2;
        rv |= (tmp >> (i * 2) & 3);
    }

    mer = rv;
    strand = (strand == Data::Strand::FORWARD ? Data::Strand::REVERSE : Data::Strand::FORWARD);
}

}  // namespace Pbmer
}  // namespace PacBio
