// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBCOPPER_PBMER_PARSER_H
#define PBCOPPER_PBMER_PARSER_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <array>
#include <string>

#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/pbmer/Mers.h>

namespace PacBio {
namespace Pbmer {

constexpr const std::array<uint8_t, 256> AsciiToDna{
    0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

class Parser
{
public:
    explicit Parser(uint8_t kmerSize);

    ///
    /// Converts a std::string into a mers (lists of forward and reverse kmers)
    ///
    Mers Parse(const std::string& dna) const;

    ///
    /// Convert a std::string into a DNABit vector, faster than `Parse`.
    ///
    std::vector<DnaBit> ParseDnaBit(const std::string& dna) const;

    ///
    /// Convert a std::string into a DNABit vector, reusing a vector.
    ///
    void ParseDnaBit(const std::string& dna, std::vector<DnaBit>& kms) const;

    ///
    /// Simple run length encoding
    ///
    std::string RLE(const std::string& dna) const;

    ///
    /// Simple destructive, in-place run length encoding
    ///
    void RLE(std::string& dna) const;

private:
    uint8_t kmerSize_;
    uint64_t mask_;
    uint64_t shift1_;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_PARSER_H
