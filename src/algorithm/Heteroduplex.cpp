#include <pbcopper/algorithm/Heteroduplex.h>

#include <pbcopper/algorithm/internal/HeteroduplexUtils.h>
#include <pbcopper/math/FishersExact.h>
#include <pbcopper/utility/Ssize.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <cassert>
#include <cstddef>

namespace PacBio {
namespace Algorithm {
namespace {

constexpr std::array<char, 5> BASES{'A', 'C', 'G', 'T', '-'};

// clang-format off
constexpr std::array<uint8_t, 256> BASE_TABLE{
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 15
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 31
    //                                                     -
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,4,  255,255,  // 47
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 63

    //  A       C                G
    255,0,  255,1,   255,255,255,2,   255,255,255,255, 255,255,255,255,  // 79
    //               T
    255,255,255,255, 3,  255,255,255, 255,255,255,255, 255,255,255,255,  // 95
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 111
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 127

    //  a       c                g
    255,0,  255,1,   255,255,255,2,   255,255,255,255, 255,255,255,255,  // 143
    //               t
    255,255,255,255, 3,  255,255,255, 255,255,255,255, 255,255,255,255,  // 159
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 175
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 191

    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 207
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 223
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 239
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,  // 255
};
// clang-format on

double CalculatePValue(const int fwdCoverageCount, const int fwdMismatchCount,
                       const int revCoverageCount, const int revMismatchCount)
{
    const int n11 = fwdCoverageCount - fwdMismatchCount;  // fwd & match
    const int n12 = revCoverageCount - revMismatchCount;  // rev & match
    const int n21 = fwdMismatchCount;                     // fwd & mismatch
    const int n22 = revMismatchCount;                     // rev & mismatch

    assert(n11 >= 0);
    assert(n12 >= 0);
    assert(n21 >= 0);
    assert(n22 >= 0);

    return Math::FishersExact(n11, n12, n21, n22);
}

}  // namespace

namespace internal {

std::pair<char, int> MostCommonBase(const BaseCount& counts, const char refBase)
{
    assert(static_cast<int>(refBase) < 256);

    // a tie goes to reference base, so the max count starts there
    int maxBaseCountIdx = BASE_TABLE[static_cast<unsigned char>(refBase)];
    int maxBaseCount = counts[maxBaseCountIdx];

    for (int i = 0; i < 5; ++i) {
        const int baseCount = counts[i];
        if (maxBaseCount < baseCount) {
            maxBaseCount = baseCount;
            maxBaseCountIdx = i;
        }
    }

    return {BASES[maxBaseCountIdx], maxBaseCount};
}

StrandCounts::StrandCounts(const std::string& ref)
    : NumReads(ref.size()), MismatchBaseCounts(ref.size()), MostCommonBases{ref}
{
    assert(ref.size() == NumReads.size());
    assert(ref.size() == MismatchBaseCounts.size());
    assert(ref.size() == MostCommonBases.size());
}

StrandRawData::StrandRawData(const size_t refLength)
    : NumReads(refLength, 0)
    , BaseCounts{refLength, BaseCount{}}
    , PotentialMismatches(refLength, 0)
    , Insertions{refLength}
{
    assert(refLength == NumReads.size());
    assert(refLength == BaseCounts.size());
    assert(refLength == PotentialMismatches.size());
    assert(refLength == Insertions.size());
}

StrandRawData CalculateStrandRawData(const std::string& reference, const StrandInput& input)
{
    assert(input.Sequences.size() == input.Cigars.size());
    assert(input.Sequences.size() == input.Positions.size());

    StrandRawData result{reference.size()};

    const int referenceSize = Utility::Ssize(reference);
    const int numSequences = Utility::Ssize(input.Sequences);
    for (int i = 0; i < numSequences; ++i) {
        const auto& cigar = input.Cigars[i];
        const auto& seq = input.Sequences[i];
        const auto startPos = input.Positions[i];

        int targetPos = startPos;
        int queryPos = 0;

        for (const auto& op : cigar) {
            const auto opType = op.Type();
            const int opLength = op.Length();

            switch (opType) {
                case Data::CigarOperationType::SEQUENCE_MATCH: {
                    for (int j = 0; j < opLength; ++j) {
                        const int8_t base = seq[queryPos + j];
                        ++result.BaseCounts[targetPos + j][BASE_TABLE[base]];
                    }
                    break;
                }
                case Data::CigarOperationType::SEQUENCE_MISMATCH: {
                    for (int j = 0; j < opLength; ++j) {
                        const int8_t base = seq[queryPos + j];
                        ++result.BaseCounts[targetPos + j][BASE_TABLE[base]];
                        result.PotentialMismatches[targetPos + j] = 1;
                    }
                    break;
                }
                case Data::CigarOperationType::DELETION: {
                    for (int j = 0; j < opLength; ++j) {
                        ++result.BaseCounts[targetPos + j][4];  // 4 = [BaseValue('-')];
                        result.PotentialMismatches[targetPos + j] = 1;
                    }
                    break;
                }
                case Data::CigarOperationType::INSERTION: {
                    if (targetPos < referenceSize) {
                        result.Insertions[targetPos].emplace_back(seq.data() + queryPos, opLength);
                    }
                    break;
                }

                // Full ignore
                case Data::CigarOperationType::SOFT_CLIP:       // fallthrough
                case Data::CigarOperationType::HARD_CLIP:       // .
                case Data::CigarOperationType::REFERENCE_SKIP:  // .
                case Data::CigarOperationType::PADDING:         // .
                    break;
                default:
                    std::ostringstream msg;
                    msg << "encountered unsupported CIGAR op: '"
                        << Data::CigarOperation::TypeToChar(opType) << "'";
                    throw std::runtime_error{msg.str()};
            }

            if (Data::ConsumesReference(opType)) {
                for (int j = 0; j < opLength; ++j) {
                    ++result.NumReads[targetPos + j];
                }
                targetPos += opLength;
            }
            if (Data::ConsumesQuery(opType)) {
                queryPos += opLength;
            }
        }
    }

    return result;
}

}  // namespace internal

HeteroduplexResults FindHeteroduplex(
    const std::string& reference, const std::vector<std::string>& fwdSequences,
    const std::vector<std::string>& revSequences, const std::vector<Data::Cigar>& fwdCigars,
    const std::vector<Data::Cigar>& revCigars, const std::vector<int32_t>& fwdPositions,
    const std::vector<int32_t>& revPositions, const HeteroduplexSettings& settings)
{
    assert(fwdSequences.size() == fwdCigars.size());
    assert(fwdSequences.size() == fwdPositions.size());
    assert(revSequences.size() == revCigars.size());
    assert(revSequences.size() == revPositions.size());
    assert(settings.MinimumPerStrandSubreadCoverage >= 0);

    // quick exit on insufficient data
    if (reference.empty() ||
        (Utility::Ssize(fwdSequences) < settings.MinimumPerStrandSubreadCoverage) ||
        (Utility::Ssize(revSequences) < settings.MinimumPerStrandSubreadCoverage)) {
        return HeteroduplexResults{};
    }

    // combine fwd and rev input
    std::vector<std::string> combinedSequences = fwdSequences;
    combinedSequences.insert(combinedSequences.end(), revSequences.begin(), revSequences.end());
    std::vector<Data::Cigar> combinedCigars = fwdCigars;
    combinedCigars.insert(combinedCigars.end(), revCigars.begin(), revCigars.end());
    std::vector<int32_t> combinedPositions = fwdPositions;
    combinedPositions.insert(combinedPositions.end(), revPositions.begin(), revPositions.end());

    // get data from CIGARs for base counts from combined strand input
    const internal::StrandRawData& combinedStrands = internal::CalculateStrandRawData(
        reference, internal::StrandInput{combinedSequences, combinedCigars, combinedPositions});

    // get string of most common bases
    const auto BaseCountsToMostCommonBasesString =
        [](const std::vector<internal::BaseCount>& counts, const std::string_view& ref) {
            std::string res;
            res.reserve(counts.size());
            for (int i = 0; i < Utility::Ssize(counts); ++i) {
                res.push_back(internal::MostCommonBase(counts[i], ref[i]).first);
            }
            return res;
        };

    // recalculate reference by counting the most common base at each position
    // since input reference does not represent the most common bases overall
    const std::string& recalculatedReference =
        BaseCountsToMostCommonBasesString(combinedStrands.BaseCounts, reference);

    // gather pileup and potential mismatch sites from CIGARs for stranded input
    const internal::StrandRawData fwdStrand = internal::CalculateStrandRawData(
        recalculatedReference, internal::StrandInput{fwdSequences, fwdCigars, fwdPositions});
    const internal::StrandRawData revStrand = internal::CalculateStrandRawData(
        recalculatedReference, internal::StrandInput{revSequences, revCigars, revPositions});

    std::string fwdMostCommonBases = recalculatedReference;
    std::string revMostCommonBases = recalculatedReference;

    std::vector<int32_t> variableSites;
    std::vector<int32_t> significantSites;
    std::vector<char> significantBases;

    const int refLength = Utility::Ssize(reference);
    const int start = std::max(1, settings.IgnoreEndBases);
    const int end = std::min(refLength - 1, refLength - settings.IgnoreEndBases);
    assert(start > 0);
    assert(end > start);
    assert(end < refLength);
    for (int i = start; i < end; ++i) {

        // nothing to see, carry on
        if ((fwdStrand.PotentialMismatches[i] == 0) && (revStrand.PotentialMismatches[i] == 0)) {
            continue;
        }

        // sanity check: ensure some coverage on both strands
        const int fwdCoverageCount = fwdStrand.NumReads[i];
        const int revCoverageCount = revStrand.NumReads[i];
        if ((fwdCoverageCount == 0) || (revCoverageCount == 0)) {
            continue;
        }

        // compute most common bases at position
        const char refBase = recalculatedReference[i];

        const auto fwdMostCommonBaseCount =
            internal::MostCommonBase(fwdStrand.BaseCounts[i], refBase);
        const char fwdMostCommonBase = fwdMostCommonBaseCount.first;
        const int fwdMostCommonCount = fwdMostCommonBaseCount.second;

        const auto revMostCommonBaseCount =
            internal::MostCommonBase(revStrand.BaseCounts[i], refBase);
        const char revMostCommonBase = revMostCommonBaseCount.first;
        const int revMostCommonCount = revMostCommonBaseCount.second;
        if (fwdMostCommonBase == revMostCommonBase) {
            continue;
        }
        assert((fwdMostCommonBase != refBase) || (revMostCommonBase != refBase));

        // skip deletion sites: the source of a large % of false positive HD calls
        if ((fwdMostCommonBase == '-') || (revMostCommonBase == '-')) {
            continue;
        }

        // mask SNPs with alleles matching up & downstream bases, respectively. e.g.
        //       v
        // fwd: AAT
        //      AAT
        // rev: ATT
        //      ATT
        if ((fwdMostCommonBase == recalculatedReference[i - 1]) &&
            (revMostCommonBase == recalculatedReference[i + 1])) {
            continue;
        }
        if ((revMostCommonBase == recalculatedReference[i - 1]) &&
            (fwdMostCommonBase == recalculatedReference[i + 1])) {
            continue;
        }

        // mask SNPs adjacent to insertions, which contain the SNP alleles
        const auto HasMostCommonBase = [fwdMostCommonBase,
                                        revMostCommonBase](const std::string_view& ins) {
            return (ins.find(fwdMostCommonBase) != std::string::npos) ||
                   (ins.find(revMostCommonBase) != std::string::npos);
        };

        const auto& fwdAdjacentInsertions = fwdStrand.Insertions[i + 1];
        const int fwdRelevantInsertions = std::count_if(
            fwdAdjacentInsertions.cbegin(), fwdAdjacentInsertions.cend(), HasMostCommonBase);
        const auto& revAdjacentInsertions = revStrand.Insertions[i + 1];
        const int revRelevantInsertions = std::count_if(
            revAdjacentInsertions.cbegin(), revAdjacentInsertions.cend(), HasMostCommonBase);

        const int numPositionInsertions =
            Utility::Ssize(fwdAdjacentInsertions) + Utility::Ssize(revAdjacentInsertions);
        const double fractionAdjacentInsertions =
            static_cast<double>(fwdRelevantInsertions + revRelevantInsertions) /
            std::max(1, numPositionInsertions);
        if (fractionAdjacentInsertions >= settings.AdjacentInsertionThreshold) {
            continue;
        }

        // store mismatch count & update "most common bases" for return
        int fwdMismatchCount = 0;
        int revMismatchCount = 0;
        if (fwdMostCommonBase != refBase) {
            fwdMismatchCount = fwdMostCommonCount;
            fwdMostCommonBases[i] = fwdMostCommonBase;
        }
        if (revMostCommonBase != refBase) {
            revMismatchCount = revMostCommonCount;
            revMostCommonBases[i] = revMostCommonBase;
        }

        // check mismatch fractions
        const double fwdMismatchFractionCoverage =
            static_cast<double>(fwdMismatchCount) / std::max(1, fwdCoverageCount);
        const double revMismatchFractionCoverage =
            static_cast<double>(revMismatchCount) / std::max(1, revCoverageCount);
        const bool meetsMinFractionCoverage =
            ((fwdMismatchFractionCoverage >= settings.MinFractionMismatchCoverage) ||
             (revMismatchFractionCoverage >= settings.MinFractionMismatchCoverage));
        if (!meetsMinFractionCoverage) {
            continue;
        }

        // store all variable sites that have met mismatch/coverage requirements
        variableSites.push_back(i);

        // determine the site's significance using Fisher's exact text
        const double pValue =
            CalculatePValue(fwdCoverageCount, fwdMismatchCount, revCoverageCount, revMismatchCount);
        if (pValue < settings.AlphaLevel) {
            significantSites.push_back(i);
            significantBases.push_back((fwdMostCommonBase == refBase) ? revMostCommonBase
                                                                      : fwdMostCommonBase);
        }
    }

    // calculate results summary & return
    const uint32_t numSignificantSites = significantSites.size();
    const uint32_t numVariableSites = variableSites.size();
    const double fractionSites =
        static_cast<double>(numSignificantSites) / std::max(1u, numVariableSites);

    return HeteroduplexResults{numSignificantSites,
                               numVariableSites,
                               static_cast<uint32_t>(refLength),
                               fractionSites,
                               std::move(variableSites),
                               std::move(significantSites),
                               std::move(significantBases),
                               std::move(fwdMostCommonBases),
                               std::move(revMostCommonBases)};
}

bool IsHeteroduplex(const std::string& reference, const std::vector<std::string>& fwdSequences,
                    const std::vector<std::string>& revSequences,
                    const std::vector<Data::Cigar>& fwdCigars,
                    const std::vector<Data::Cigar>& revCigars,
                    const std::vector<int32_t>& fwdPositions,
                    const std::vector<int32_t>& revPositions, const HeteroduplexSettings& settings)
{
    const HeteroduplexResults results =
        FindHeteroduplex(reference, fwdSequences, revSequences, fwdCigars, revCigars, fwdPositions,
                         revPositions, settings);
    return results.FractionSites >= settings.MinFractionSites;
}

}  // namespace Algorithm
}  // namespace PacBio
