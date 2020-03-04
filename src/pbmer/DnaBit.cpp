// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#include <pbcopper/pbmer/DnaBit.h>

#include <array>
#include <tuple>

#include <pbcopper/pbmer/Parser.h>

namespace PacBio {
namespace Pbmer {

uint64_t Mix64Masked(const uint64_t key, const uint8_t kmerSize)
{
    uint64_t res = key;
    res = (~res + (res << 21)) & ((1ull << 2 * kmerSize) - 1);  // key = (key << 21) - key - 1;
    res = res ^ (res >> 24);
    res = ((res + (res << 3)) + (res << 8)) & ((1ull << 2 * kmerSize) - 1);  // key * 265
    res = res ^ (res >> 14);
    res = ((res + (res << 2)) + (res << 4)) & ((1ull << 2 * kmerSize) - 1);  // key * 21
    res = res ^ (res >> 28);
    res = (res + (res << 31)) & ((1ull << 2 * kmerSize) - 1);
    return res;
}

// thanks to https://www.biostars.org/p/113640/ <- and a slight mod!
uint64_t ReverseComp64(const uint64_t mer, const uint8_t kmerSize)
{
    uint64_t res = ~mer;

    res = ((res >> 2 & 0x3333333333333333) | (res & 0x3333333333333333) << 2);
    res = ((res >> 4 & 0x0F0F0F0F0F0F0F0F) | (res & 0x0F0F0F0F0F0F0F0F) << 4);
    res = ((res >> 8 & 0x00FF00FF00FF00FF) | (res & 0x00FF00FF00FF00FF) << 8);
    res = ((res >> 16 & 0x0000FFFF0000FFFF) | (res & 0x0000FFFF0000FFFF) << 16);
    res = ((res >> 32 & 0x00000000FFFFFFFF) | (res & 0x00000000FFFFFFFF) << 32);

    return (res >> (2 * (32 - kmerSize)));
}

DnaBit::DnaBit() = default;

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

void DnaBit::AppendBase(const uint8_t b)
{
    mer = ((mer << 2) & ((uint64_t)-1 >> (64 - (2 * msize)))) | (b % 4);
}

void DnaBit::AppendBase(const char b)
{
    mer = ((mer << 2) & ((uint64_t)-1 >> (64 - (2 * msize)))) | AsciiToDna[b];
}

void DnaBit::Bin2DnaBit(BI bin)
{
    msize = static_cast<uint8_t>(bin >> 48);
    strand = static_cast<uint8_t>(bin >> 56);
    mer = static_cast<uint64_t>(bin >> 64);
}

BI DnaBit::DnaBit2Bin() const
{
    BI rv = 0;
    rv |= mer;
    rv <<= 8;
    rv |= strand;
    rv <<= 8;
    rv |= msize;
    rv <<= 48;

    return rv;
}

uint8_t DnaBit::GetFirstBaseIdx() const
{
    return static_cast<uint8_t>(((mer >> ((2 * msize) - 2)) & 3));
}
uint8_t DnaBit::GetFirstBaseRCIdx() const { return 3 & ~GetLastBaseIdx(); }

uint8_t DnaBit::GetLastBaseIdx() const { return static_cast<uint8_t>(mer & 3); }

uint8_t DnaBit::GetLastBaseRCIdx() const { return 3 & ~GetFirstBaseIdx(); }

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

DnaBit DnaBit::LexSmallerEq(void) const
{
    DnaBit rc = *this;
    rc.ReverseComp();
    if (rc <= *this) {
        return rc;
    }
    return *this;
}

void DnaBit::PrependBase(const uint8_t b)
{
    mer = (uint64_t(b % 4) << 2 * (msize - 1)) | (mer >> 2);
}

void DnaBit::PrependBase(const char b)
{
    mer = (uint64_t(AsciiToDna[b]) << 2 * (msize - 1)) | (mer >> 2);
}

void DnaBit::ReverseComp(void)
{
    mer = ReverseComp64(mer, msize);
    strand = !strand;
}

}  // namespace Pbmer
}  // namespace PacBio
