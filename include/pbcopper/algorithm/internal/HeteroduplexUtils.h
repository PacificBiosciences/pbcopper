#ifndef PBCOPPER_ALGORITHM_HETERODUPLEXUTILS_H
#define PBCOPPER_ALGORITHM_HETERODUPLEXUTILS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Cigar.h>
#include <pbcopper/data/Strand.h>

#include <array>
#include <string>
#include <vector>

#include <cstdint>

namespace PacBio {
namespace Algorithm {
namespace internal {

// ----------------
// base counts
// ----------------

// A:0, C:1, G:2, T:3, -:4
using BaseCount = std::array<int, 5>;

///
/// \brief Return number of bases at position (i.e. excludes deletions)
///
int CoverageCount(const BaseCount& baseCounts);

///
/// Return total number of bases that do not match reference base
/// \note Ignores deletion count
///
int MismatchCount(const BaseCount& baseCounts, char ref);

// ----------------
// CIGAR counts
// ----------------

// Data members are all of ref.size() length. So every reference position has
// a value. Query insertions are ignored.
struct CigarCounts
{
    CigarCounts() = default;
    explicit CigarCounts(const std::string& ref);

    std::vector<int> NumReads;  // includes reads with deletion at position
    std::vector<int> MismatchBaseCounts;
    std::string MostCommonBases;
    std::vector<BaseCount> BaseCounts;
};

CigarCounts CigarMismatchCounts(const std::string& reference,
                                const std::vector<std::string>& sequences,
                                const std::vector<Data::Cigar>& cigars,
                                const std::vector<int32_t>& positions);

// -------------------
// pileup processing
// -------------------

struct PileUpInfo
{
    bool HomopolymerAdjacent = false;
    bool UsingAdjacent = false;
    Data::Strand StrandToUpdate = Data::Strand::UNMAPPED;
};

PileUpInfo PileupContext(char fwdMostCommonBase, const std::string& fwdMostCommonBases,
                         char revMostCommonBase, const std::string& revMostCommonBases,
                         int fwdCoverageCount, int revCoverageCount, int fwdMismatchCount,
                         int revMismatchCount, int i, int refLength, int homopolymerThreshold);

}  // namespace internal
}  // namespace Algorithm
}  // namespace PacBio

#endif  // PBCOPPER_ALGORITHM_HETERODUPLEXUTILS_H
