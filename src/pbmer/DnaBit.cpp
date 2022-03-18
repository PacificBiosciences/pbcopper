#include <pbcopper/pbmer/DnaBit.h>

#include <pbcopper/align/EdlibAlign.h>
#include <pbcopper/container/Unordered.h>
#include <pbcopper/pbmer/Parser.h>
#include <pbcopper/utility/Intrinsics.h>  // PopCount
#include <pbcopper/utility/MinMax.h>

#include <array>
#include <iostream>
#include <limits>
#include <memory>  // std::unique_ptr<[]>
#include <numeric>
#include <tuple>
#include <vector>

#include <cassert>
#include <cstdint>

namespace PacBio {
namespace Pbmer {

// all bits set to one for a uint64_t
constexpr uint64_t allOn = std::numeric_limits<uint64_t>::max();

void DnaBit::SetBase(const char c, const int position)
{
    assert(position >= 0);
    assert(position < msize);

    const uint64_t base = AsciiToDna[c];
    const int step = position * 2;

    mer = (mer & ~(0b11ULL << step)) | (base << step);
}

// use a lower and (implicit) upper bitmask to squeeze out a base.
void DnaBit::DeleteBase(const int position)
{
    assert(position >= 0);
    assert(position < msize);

    //             example position = 1 ─┐
    //                                   v
    //                          4  3  2  1  0
    //   mer:                   T  C  A  T  G
    //   binary:               11'01'00'11'10
    //
    //   lowerBitMask:         00'00'00'00'11
    //
    // bases before position:
    //   (mer & lowerBitMask): 00'00'00'00'10 (lower)
    //
    // bases past position:
    //   (mer >> 2):           00'11'01'00'11
    //   (~lowerBitMask):      11'11'11'11'00
    //   bit-&:                00'11'01'00'00 (upper)
    //
    //                             3  2  1  0
    // lower | upper:          00'11'01'00'10
    //                             T  C  A  G
    const uint64_t lowerBitMask{(1ull << (2 * position)) - 1};

    mer = (mer & lowerBitMask) | ((mer >> 2) & (~lowerBitMask));
    --msize;
}

void DnaBit::InsertBase(const char c, const int position)
{
    assert(position >= 0);
    assert(position <= msize);
    assert(position < 32);

    const uint64_t lowerBitMask{(1ull << (2 * position)) - 1};

    mer = (mer & lowerBitMask) | ((mer & ~lowerBitMask) << 2);
    ++msize;

    SetBase(c, position);
}

// https://gist.github.com/badboy/6267743
// This is a multiplication method for hashing. Minimap2 uses this function, but
// this function does not mask, no constraining the bitrange.
uint64_t Hash64shift(uint64_t key)
{
    key = (~key) + (key << 21);  // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);  // key * 265
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);  // key * 21
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}

uint64_t Mix64Masked(const uint64_t key, const uint8_t kmerSize) noexcept
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
    assert((0 < kmerSize) && (kmerSize <= 32));

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

bool DnaBit::operator<(const DnaBit& b) const noexcept
{
    return Mix64Masked(mer, msize) < Mix64Masked(b.mer, msize);
}

bool DnaBit::operator>(const DnaBit& b) const noexcept { return b < *this; }

bool DnaBit::operator>=(const DnaBit& b) const noexcept { return !(*this < b); }

bool DnaBit::operator<=(const DnaBit& b) const noexcept { return !(*this > b); }

bool DnaBit::operator==(const DnaBit& b) const noexcept
{
    return std::tie(mer, strand) == std::tie(b.mer, b.strand);
}

bool DnaBit::operator!=(const DnaBit& b) const noexcept { return !(*this == b); }

void DnaBit::AppendBase(const uint8_t b)
{
    mer = ((mer << 2) & (allOn >> (64 - (2 * msize)))) | (b % 4);
}

void DnaBit::AppendBase(const char b)
{
    mer = ((mer << 2) & (allOn >> (64 - (2 * msize)))) | AsciiToDna[b];
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

uint8_t DnaBit::FirstBaseIdx() const
{
    return static_cast<uint8_t>(((mer >> ((2 * msize) - 2)) & 3));
}
uint8_t DnaBit::FirstBaseRCIdx() const { return 3 & ~LastBaseIdx(); }

uint8_t DnaBit::LastBaseIdx() const { return static_cast<uint8_t>(mer & 3); }

uint8_t DnaBit::LastBaseRCIdx() const { return 3 & ~FirstBaseIdx(); }

uint64_t DnaBit::HashedKmer() const { return Hash64shift(mer); }

uint64_t DnaBit::RCHashedKmer() const { return Mix64Masked(ReverseComp64(mer, msize), msize); }

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
    if (rc.mer <= mer) {
        return rc;
    }
    return *this;
}

DnaBit DnaBit::LexSmallerEqHashed(void) const
{
    DnaBit rc = *this;
    rc.ReverseComp();
    if (rc <= *this) {
        return rc;
    }
    return *this;
}

uint64_t DnaBit::LexSmallerEq64() const
{
    uint64_t rc = ReverseComp64(mer, msize);
    if (rc < mer) {
        return rc;
    }
    return mer;
}

uint64_t DnaBit::LexSmallerEq64Hashed() const
{
    uint64_t rc = ReverseComp64(mer, msize);
    if (Mix64Masked(rc, msize) <= Mix64Masked(mer, msize)) {
        return rc;
    }
    return mer;
}

void DnaBit::MakeLexSmaller()
{
    uint64_t rc = ReverseComp64(mer, msize);
    if (rc <= mer) {
        mer = rc;
        strand = !strand;
    }
}

void DnaBit::MakeLexSmallerHashed()
{
    DnaBit test = *this;
    test.ReverseComp();
    if (test <= *this) {
        *this = test;
    }
}

uint8_t DnaBit::LongestDiNucRun() const
{
    if (msize <= 1) {
        return 0;
    }
    if (msize <= 3) {
        return 1;
    }

    uint8_t runLength = 0;
    uint8_t tmpRun = 1;

    // four bit mask
    constexpr uint8_t mask = 0b1111;
    uint8_t i = 0;
    uint64_t tmp1 = 0;
    uint64_t tmp2 = 0;
    uint8_t upperBound = (msize * 2) - 4;

    //first frame
    while (i < upperBound) {
        tmp1 = mer;
        tmp2 = mer;

        tmp1 = (tmp1 >> i);
        tmp2 = (tmp2 >> (i + 4));

        if ((tmp1 & mask) == (tmp2 & mask)) {
            ++tmpRun;
            if (tmpRun > runLength) {
                runLength = tmpRun;
            }
        } else {
            tmpRun = 1;
        }
        i += 4;
    }

    tmpRun = 1;

    //second frame
    while (i < upperBound) {
        tmp1 = (mer >> 2);
        tmp2 = (mer >> 2);

        tmp1 = (tmp1 >> i);
        tmp2 = (tmp2 >> (i + 4));

        if ((tmp1 & mask) == (tmp2 & mask)) {
            ++tmpRun;
            if (tmpRun > runLength) {
                runLength = tmpRun;
            }
        } else {
            tmpRun = 1;
        }
        i += 4;
    }

    return runLength;
}

void DnaBit::PrependBase(const uint8_t b)
{
    mer = (uint64_t(b % 4) << 2 * (msize - 1)) | (mer >> 2);
}

void DnaBit::PrependBase(const char b)
{
    assert(msize > 0);
    mer = (uint64_t(AsciiToDna[b]) << 2 * (msize - 1)) | (mer >> 2);
}

void DnaBit::ReverseComp(void)
{
    mer = ReverseComp64(mer, msize);
    strand = !strand;
}

std::string DnaBitVec2String(const std::vector<DnaBit>& bits)
{

    std::string rv;
    if (bits.empty()) {
        return rv;
    }
    rv = bits.front().KmerToStr();
    if (bits.size() == 1) {
        return rv;
    }
    const uint8_t lastStrand = bits.front().strand;
    for (size_t i = 1; i < bits.size(); ++i) {
        DnaBit d = bits[i];
        if (d.strand != lastStrand) {
            d.ReverseComp();
        }
        if (!d.KmerToStr().empty()) {
            rv += d.KmerToStr().back();
        }
    }
    return rv;
}

std::vector<DnaBit> DnaBit::Neighbors() const
{
    std::vector<DnaBit> results;
    results.reserve(4 * msize);
    Container::UnorderedSet<uint64_t> seen;

    for (int i = 0; i < msize; ++i) {
        for (int j = 0; j < 4; ++j) {
            DnaBit currentMer = *this;
            currentMer.SetBase(j, i);
            const auto insertResult = seen.insert(currentMer.mer);
            if (insertResult.second) {
                results.emplace_back(currentMer);
            }
        }
    }
    return results;
}
uint64_t DnaBit::BitMask() const noexcept { return uint64_t(-1) >> (64 - 2 * msize); }

int DnaBit::HammingDistance(const DnaBit other) const noexcept
{
    if (other.msize != msize) {
        return -1;
    }
    // You can speed this up a bit by caching the bitmask
    // and anding it with 0x5555555555555555,
    // but that requires storing in memory.
    // I would hope that the compiler can do this for us. - DB
    uint64_t x0 = ~(mer ^ other.mer);
    // Now we want to find bit pairs of 0 in x ^ y
    uint64_t x1 = (x0 >> 1);
    uint64_t x2 = x1 & (0x5555555555555555);
    return msize - Utility::PopCount(x0 & x2 & BitMask());
}

int HammingDistance(const uint64_t x, const uint64_t y, int k) noexcept
{
    const uint64_t bitMask = (uint64_t(0x5555555555555555) >> (2 * k));
    const uint64_t x0 = ~(x ^ y);
    return k - Utility::PopCount(x0 & (x0 >> 1) & bitMask);
}

int DnaBit::EditDistance(const DnaBit other) const noexcept
{
#if 0
    // This edit distance code seems to be slightly off.
    // We have to pay a little cost to convert the DnaBits to std::strings
    // but then we have the 
    const int m = msize, n = other.msize;
    auto scratch = std::make_unique<int8_t[]>((n + 1) * 2);
    int8_t* row1 = scratch.get();
    int8_t* row2 = row1 + n + 1;
    std::iota(row1, row1 + n + 1, 0);
    for (int i = 0; i < m; ++i) {
        row2[i] = i + 1;
        const int leftHandValue = (mer >> (i * 2)) & 3;
        for (int j = 0; j < n; ++j) {
            const int rightHandValue = (other.mer >> (j * 2)) & 3;
            const int deleteCost = row1[j + 1] + 1;
            const int insertionCost = row2[j] + 1;
            const int substitutionCost = row1[j] + (rightHandValue != leftHandValue);
            row2[j + 1] = Utility::Min(deleteCost, insertionCost, substitutionCost);
        }
        std::swap(row1, row2);
    }
    return row1[msize];
#else
    const std::string myStr = KmerToStr();
    const std::string oStr = other.KmerToStr();
    EdlibAlignConfig conf{int(std::max(oStr.size(), myStr.size())), EDLIB_MODE_NW,
                          EDLIB_TASK_DISTANCE, nullptr, 0};
    return Align::EdlibAlign(myStr, oStr, conf).Data.editDistance;
#endif
}

}  // namespace Pbmer
}  // namespace PacBio
