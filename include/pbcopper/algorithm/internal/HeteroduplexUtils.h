#ifndef PBCOPPER_ALGORITHM_HETERODUPLEXUTILS_H
#define PBCOPPER_ALGORITHM_HETERODUPLEXUTILS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Cigar.h>

#include <array>
#include <string>
#include <string_view>
#include <utility>
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

/// Find most common base and count (tie goes to the reference base)
///
std::pair<char, int> MostCommonBase(const BaseCount& counts, char refBase);

// ----------------
// CIGAR counts
// ----------------

///
/// Aggregate strand counts input
///
struct StrandInput
{
    const std::vector<std::string>& Sequences;
    const std::vector<Data::Cigar>& Cigars;
    const std::vector<int32_t>& Positions;
};

///
/// Contains results of initial CIGAR scan, with raw counts at each reference
/// position
///
struct StrandRawData
{
    explicit StrandRawData(size_t refLength);

    std::vector<int> NumReads;  // includes reads with deletion at position
    std::vector<BaseCount> BaseCounts;
    std::vector<uint8_t> PotentialMismatches;
    std::vector<std::vector<std::string_view>> Insertions;
};

///
/// Contains most common base and potential mismatch counts for each reference
/// position
///
struct StrandCounts
{
    StrandCounts() = default;
    explicit StrandCounts(const std::string& ref);

    std::vector<int> NumReads;  // includes reads with deletion at position
    std::vector<int> MismatchBaseCounts;
    std::string MostCommonBases;
};

// gather mismatches, base counts, etc for a single strand
StrandRawData CalculateStrandRawData(const std::string& reference, const StrandInput& input);

}  // namespace internal
}  // namespace Algorithm
}  // namespace PacBio

#endif  // PBCOPPER_ALGORITHM_HETERODUPLEXUTILS_H
