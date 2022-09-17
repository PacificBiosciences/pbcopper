#ifndef PBCOPPER_ALGORITHM_HETERODUPLEX_H
#define PBCOPPER_ALGORITHM_HETERODUPLEX_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Cigar.h>

#include <string>
#include <vector>

#include <cstdint>

namespace PacBio {
namespace Algorithm {

struct HeteroduplexSettings
{
    // clang-format off
    struct Defaults
    {
        static constexpr double ALPHA_LEVEL = 0.1;
        static constexpr double MIN_FRACTION_SITES = 0.01;
        static constexpr double MIN_FRACTION_MISMATCH_COVERAGE = 0.7;
        static constexpr int HOMOPOLYMER_THRESHOLD = 3;
        static constexpr double MIN_SECONDARY_FRACTION = 0.40;
        static constexpr int IGNORE_END_BASES = 25;
        static constexpr int MINIMUM_PER_STRAND_SUBREAD_COVERAGE = 5;
        static constexpr bool SKIP_DELETIONS = true;
        static constexpr double ADJACENT_INSERTION_THRESHOLD = 0.75;
    };
    // clang-format on

    double AlphaLevel = Defaults::ALPHA_LEVEL;
    double MinFractionSites = Defaults::MIN_FRACTION_SITES;
    double MinFractionMismatchCoverage = Defaults::MIN_FRACTION_MISMATCH_COVERAGE;
    int HomopolymerThreshold = Defaults::HOMOPOLYMER_THRESHOLD;
    double MinSecondaryFraction = Defaults::MIN_SECONDARY_FRACTION;
    int IgnoreEndBases = Defaults::IGNORE_END_BASES;
    int MinimumPerStrandSubreadCoverage = Defaults::MINIMUM_PER_STRAND_SUBREAD_COVERAGE;
    bool SkipDeletions = Defaults::SKIP_DELETIONS;
    double AdjacentInsertionThreshold = Defaults::ADJACENT_INSERTION_THRESHOLD;
};

struct HeteroduplexResults
{
    uint32_t NumSignificantSites = 0;  // count of variable sites with p-value below alpha level
    uint32_t NumVariableSites = 0;     // total number of variable sites
    uint32_t SequenceLength = 0;       // length of consensus sequence used
    double FractionSites = 0.0;        // numSignificantSites / numVariableSites

    std::vector<int32_t> VariableSites;
    std::vector<int32_t> SignificantSites;
    std::vector<char> SignificantBases;

    std::string FwdMostCommonBases;
    std::string RevMostCommonBases;
};

///
/// \brief Determine heteroduplex finder counts/stats
///
HeteroduplexResults FindHeteroduplex(const std::string& reference,
                                     const std::vector<std::string>& fwdSequences,
                                     const std::vector<std::string>& revSequences,
                                     const std::vector<Data::Cigar>& fwdCigars,
                                     const std::vector<Data::Cigar>& revCigars,
                                     const std::vector<int32_t>& fwdPositions,
                                     const std::vector<int32_t>& revPositions,
                                     const HeteroduplexSettings& settings = HeteroduplexSettings{});

///
/// \brief Convenience method wrapping FindHeteroduplex
///
/// \return true if the percent of significant sites is greater than minimum
///         required
///
bool IsHeteroduplex(const std::string& reference, const std::vector<std::string>& fwdSequences,
                    const std::vector<std::string>& revSequences,
                    const std::vector<Data::Cigar>& fwdCigars,
                    const std::vector<Data::Cigar>& revCigars,
                    const std::vector<int32_t>& fwdPositions,
                    const std::vector<int32_t>& revPositions,
                    const HeteroduplexSettings& settings = HeteroduplexSettings{});

}  // namespace Algorithm
}  // namespace PacBio

#endif  // PBCOPPER_ALGORITHM_HETERODUPLEX_H
