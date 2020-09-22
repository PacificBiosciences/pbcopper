// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBCOPPER_PBMER_DNABIT_H
#define PBCOPPER_PBMER_DNABIT_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <string>
#include <vector>

namespace PacBio {
namespace Pbmer {

__extension__ using BI = __int128;

class DnaBit
{
public:
    // 32 DNA bases are packed into a uint64 (<<), empty slots are on the left.
    uint64_t mer = 0;
    // Strand and size (in bp; bits = 2*bp).
    // Keeping the mer size in the class makes life a lot easier.
    // 0:+ forward strand ; 1:- reverse strand
    uint8_t strand = 0;
    uint8_t msize = 0;

    DnaBit();
    DnaBit(uint64_t k, uint8_t strand, uint8_t size);

    // Ignores strand, compares hashed kmer.
    bool operator>(DnaBit const& b) const noexcept;
    bool operator<(DnaBit const& b) const noexcept;
    bool operator>=(DnaBit const& b) const noexcept;
    bool operator<=(DnaBit const& b) const noexcept;

    // Checks strand, not hashed kmer.
    bool operator==(DnaBit const& b) const noexcept;
    bool operator!=(DnaBit const& b) const noexcept;

    ///
    /// \return the hashed kmer
    ///
    uint64_t HashedKmer() const;

    ///
    /// \return the rc hashed kmer
    ///
    uint64_t RCHashedKmer() const;

    ///
    /// \return the smaller DnaBit (forward/reverse).
    ///
    DnaBit LexSmallerEq() const;

    ///
    /// \return the hashed smaller DnaBit (forward/reverse).
    ///
    DnaBit LexSmallerEqHashed() const;

    ///
    /// \return the smaller uint64_t (forward/reverse).
    ///
    uint64_t LexSmallerEq64() const;

    ///
    /// \return the hashed smaller uint64_t (forward/reverse).
    ///
    uint64_t LexSmallerEq64Hashed() const;

    ///
    /// \places the smaller kmer (forward/reverse).
    ///
    void MakeLexSmaller();

    ///
    /// \places the hashed smaller kmer (forward/reverse).
    ///
    void MakeLexSmallerHashed();

    ///
    /// Count the longest di nuclotide run (AA,AT,AC,...)
    ///
    uint8_t LongestDiNucRun() const;

    ///
    /// \return the kmer as a printable string;
    ///
    std::string KmerToStr() const;

    ///
    /// Reverse complements the kmer in place.
    //
    void ReverseComp();

    ///
    /// Put a base at the end of the kmer
    //
    void AppendBase(const uint8_t c);
    void AppendBase(const char c);

    ///
    /// Put a base at the beginning of kmer
    ///
    void PrependBase(const uint8_t c);
    void PrependBase(const char c);

    ///
    /// Get the first idx
    /// \note see constexpr const std::array<char, 8> bases in cpp
    ///
    uint8_t FirstBaseIdx() const;

    ///
    /// Get the last idx
    /// \note see constexpr const std::array<char, 8> bases in cpp
    ///
    uint8_t LastBaseIdx() const;

    ///
    /// Get the first idx
    /// \note see constexpr const std::array<char, 8> bases in cpp
    ///
    uint8_t FirstBaseRCIdx() const;

    ///
    /// Get the last idx
    /// \note see constexpr const std::array<char, 8> bases in cpp
    ///
    uint8_t LastBaseRCIdx() const;

    ///
    /// Pack a DnaBit to an int128
    ///
    BI DnaBit2Bin() const;

    ///
    /// Unpack top 80 bits into a DnaBit
    ///
    void Bin2DnaBit(BI bin);
};

// This should remain a function, it has a lot of general utility.
uint64_t ReverseComp64(const uint64_t mer, uint8_t kmerSize);
// This should remain a function, it has a lot of general utility.
uint64_t Mix64Masked(uint64_t key, uint8_t kmerSize) noexcept;

///
/// Turn path to a DNA string.
///
/// \param bits    vector - DnaBit
///
/// \return a string of dna
std::string DnaBitVec2String(const std::vector<DnaBit>& bits);

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_DNABIT_H
