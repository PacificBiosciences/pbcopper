// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#include <array>
#include <tuple>

#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/pbmer/Parser.h>

namespace PacBio {
namespace Pbmer {

uint64_t Mix64Masked(uint64_t key, uint8_t kmerSize)
{
    const uint64_t mask = (1ull << 2 * kmerSize) - 1;

    key = (~key + (key << 21)) & mask;  // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = ((key + (key << 3)) + (key << 8)) & mask;  // key * 265
    key = key ^ (key >> 14);
    key = ((key + (key << 2)) + (key << 4)) & mask;  // key * 21
    key = key ^ (key >> 28);
    key = (key + (key << 31)) & mask;
    return key;
}

uint64_t ReverseComp64(const uint64_t mer, uint8_t kmerSize)
{
    const uint64_t tmp = ~mer;
    uint64_t rv = 0;

    // Thanks C.D. for purging the if statement ...
    for (size_t i = 0; i < kmerSize; ++i) {
        rv <<= 2;
        rv |= ((tmp >> (i * 2)) & 3);
    }

    return rv;
}

DnaBit::DnaBit(uint64_t k, uint8_t t, uint8_t i) : mer{k}, strand{t}, msize{i} {}

bool DnaBit::operator<(const DnaBit& b) const
{
    return Mix64Masked(mer, msize) < Mix64Masked(b.mer, msize);
}

bool DnaBit::operator>(const DnaBit& b) const { return b < *this; }

bool DnaBit::operator>=(const DnaBit& b) const { return !(*this < b); }

bool DnaBit::operator<=(const DnaBit& b) const { return !(*this > b); }

bool DnaBit::operator==(const DnaBit& b) const
{
    return std::tie(mer, strand) == std::tie(b.mer, b.strand);
}

bool DnaBit::operator!=(const DnaBit& b) const { return !(*this == b); }

DnaBit DnaBit::LexSmallerEq(void) const
{
    DnaBit rc = *this;
    rc.ReverseComp();
    if (rc <= *this) {
        return rc;
    }
    return *this;
}

void DnaBit::ReverseComp(void)
{
    mer = ReverseComp64(mer, msize);
    if (strand == 0) {
        strand = 1;
    } else {
        strand = 0;
    }
}

std::string DnaBit::KmerToStr(void) const
{
    std::string bases;
    bases.resize(msize);

    constexpr const std::array<char, 4> lookup{'A', 'C', 'G', 'T'};
    uint8_t i = 0;
    uint64_t tmp = 0;
    uint64_t offset = 0;

    while (i < msize) {
        tmp = mer;
        offset = (msize - i - 1) * 2;
        tmp >>= offset;
        bases[i] = lookup[3ull & tmp];
        ++i;
    }

    return bases;
}

bool DnaBit::AppendBase(const char b)
{
    uint8_t c = AsciiToDna[static_cast<int>(b)];
    if (c < 4) {
        uint64_t mask = (uint64_t)-1 >> (64 - (2 * msize));
        mer = ((mer << 2) & mask) | c;
        return true;
    }
    return false;
}

bool DnaBit::PrependBase(const char b)
{
    uint8_t c = AsciiToDna[static_cast<int>(b)];
    if (c < 4) {
        mer = (uint64_t(c) << 2 * (msize - 1)) | (mer >> 2);
        return true;
    }
    return false;
}

}  // namespace Pbmer
}  // namespace PacBio
