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
        static constexpr double AlphaLevel = 0.1;
        static constexpr double MinFractionSites = 0.01;
        static constexpr double MinFractionMismatchCoverage = 0.7;
        static constexpr int HomopolymerThreshold = 3;
        static constexpr double MinSecondaryFraction = 0.40;
        static constexpr int IgnoreEndBases = 25;
        static constexpr int MinimumPerStrandSubreadCoverage = 5;
        static constexpr bool SkipDeletions = false;
    };
    // clang-format on

    double AlphaLevel = Defaults::AlphaLevel;
    double MinFractionSites = Defaults::MinFractionSites;
    double MinFractionMismatchCoverage = Defaults::MinFractionMismatchCoverage;
    int HomopolymerThreshold = Defaults::HomopolymerThreshold;
    double MinSecondaryFraction = Defaults::MinSecondaryFraction;
    int IgnoreEndBases = Defaults::IgnoreEndBases;
    int MinimumPerStrandSubreadCoverage = Defaults::MinimumPerStrandSubreadCoverage;
    bool SkipDeletions = Defaults::SkipDeletions;
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
