#include <pbcopper/algorithm/Heteroduplex.h>

#include <pbcopper/algorithm/internal/HeteroduplexUtils.h>
#include <pbcopper/data/Cigar.h>
#include <pbcopper/data/Strand.h>
#include <pbcopper/logging/Logging.h>
#include <pbcopper/math/FishersExact.h>
#include <pbcopper/utility/SequenceUtils.h>
#include <pbcopper/utility/Ssize.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <cassert>

namespace PacBio {
namespace Algorithm {

using BaseCount = internal::BaseCount;
using CigarCounts = internal::CigarCounts;
using PileupInfo = internal::PileUpInfo;

namespace {

constexpr std::array<char, 5> Bases{'A', 'C', 'G', 'T', '-'};

// clang-format off
constexpr std::array<uint8_t, 256> BaseTable{
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

int CoverageCount(const BaseCount& baseCounts)
{
    return baseCounts[0] + baseCounts[1] + baseCounts[2] + baseCounts[3];
}

int MismatchCount(const BaseCount& baseCounts, const char ref)
{
    const int index = BaseTable[static_cast<int>(ref)];
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        if (i != index) {
            count += baseCounts[i];
        }
    }
    return count;
}

std::pair<char, int> MostCommonBase(const BaseCount& counts, const char refBase)
{
    assert(static_cast<int>(refBase) < 256);

    // a tie goes to reference base, so the max count starts there
    int maxBaseCountIdx = BaseTable[static_cast<int>(refBase)];
    int maxBaseCount = counts[maxBaseCountIdx];

    for (int i = 0; i < 5; ++i) {
        const int baseCount = counts[i];
        if (maxBaseCount < baseCount) {
            maxBaseCount = baseCount;
            maxBaseCountIdx = i;
        }
    }

    return {Bases[maxBaseCountIdx], maxBaseCount};
}

CigarCounts::CigarCounts(const std::string& ref)
    : NumReads(ref.size())
    , MismatchBaseCounts(ref.size())
    , MostCommonBases{ref}
    , BaseCounts{ref.size(), BaseCount{}}
{
    assert(ref.size() == NumReads.size());
    assert(ref.size() == MismatchBaseCounts.size());
    assert(ref.size() == MostCommonBases.size());
    assert(ref.size() == BaseCounts.size());
}

CigarCounts CigarMismatchCounts(const std::string& reference,
                                const std::vector<std::string>& sequences,
                                const std::vector<Data::Cigar>& cigars,
                                const std::vector<int32_t>& positions)
{
    assert(sequences.size() == cigars.size());
    assert(sequences.size() == positions.size());
    if (reference.empty() || sequences.empty()) {
        return {};
    }

    CigarCounts result{reference};

    std::vector<BaseCount> baseCounts(reference.size());
    std::vector<uint8_t> potentialMismatchPositions(reference.size());
    std::vector<uint8_t> potentialDeletionPositions(reference.size());

    for (int i = 0; i < Utility::Ssize(cigars); ++i) {
        const auto& cigar = cigars[i];
        const auto& seq = sequences[i];
        const auto startPos = positions[i];

        int targetPos = startPos;
        int queryPos = 0;

        for (const auto& op : cigar) {
            const auto opType = op.Type();
            const int opLength = op.Length();

            switch (opType) {
                case Data::CigarOperationType::SEQUENCE_MATCH: {
                    for (int j = 0; j < opLength; ++j) {
                        const int8_t base = seq[queryPos + j];
                        ++baseCounts[targetPos + j][BaseTable[base]];
                    }
                    break;
                }
                case Data::CigarOperationType::SEQUENCE_MISMATCH: {
                    for (int j = 0; j < opLength; ++j) {
                        const int8_t base = seq[queryPos + j];
                        ++baseCounts[targetPos + j][BaseTable[base]];
                        potentialMismatchPositions[targetPos + j] = 1;
                    }
                    break;
                }
                case Data::CigarOperationType::DELETION: {
                    for (int j = 0; j < opLength; ++j) {
                        ++baseCounts[targetPos + j][4];  // 4 = [BaseValue('-')];
                        potentialMismatchPositions[targetPos + j] = 1;
                    }
                    break;
                }

                // Unused, for now at least
                case Data::CigarOperationType::INSERTION: {
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

    result.BaseCounts = baseCounts;

    // Check most common base for mismatch.
    for (size_t pos = 0; pos < reference.size(); ++pos) {
        if (potentialMismatchPositions[pos] == 0) {
            continue;
        }
        const char refBase = reference[pos];
        const auto& baseCount = baseCounts[pos];

        // C++17 change to:
        // const auto[mostCommonBase, mostCommonCount] = MostCommonBase(baseCount, refBase);
        const auto mostCommonBaseResult = MostCommonBase(baseCount, refBase);
        const char mostCommonBase = mostCommonBaseResult.first;
        const int mostCommonCount = mostCommonBaseResult.second;

        if (mostCommonBase != refBase) {
            result.MostCommonBases[pos] = mostCommonBase;
            result.MismatchBaseCounts[pos] = mostCommonCount;
        }
    }

    return result;
}

PileUpInfo PileupContext(const char fwdMostCommonBase, const std::string& fwdMostCommonBases,
                         const char revMostCommonBase, const std::string& revMostCommonBases,
                         const int fwdCoverageCount, const int revCoverageCount,
                         const int fwdMismatchCount, const int revMismatchCount, int i,
                         int refLength, const int homopolymerThreshold)
{
    PileUpInfo result;

    // skip inspection at sequence ends (even if IgnoreEndBases == 0)
    if ((i == 0) || (i == refLength - 1)) {
        return result;
    }

    if (fwdMostCommonBase == '-') {
        assert(revMostCommonBase != '-');

        // in the case of extreme deletion coverage, do not adjust
        // (even if adjacent to homopolymer)
        const auto fwdDelFraction =
            static_cast<double>(fwdMismatchCount) / std::max(1, fwdCoverageCount);
        const auto revMatchFraction = static_cast<double>(revCoverageCount - revMismatchCount) /
                                      std::max(1, revCoverageCount);
        if ((fwdDelFraction > 0.75) && (revMatchFraction < 0.25)) {
            return result;
        }

        // deletion is the most common "base", but not overwhelmingly so
        // scan ahead to see if we're adjacent to a homopolymer
        const char nextBase = fwdMostCommonBases[i + 1];
        int hpCount = 1;
        const int numBases = Utility::Ssize(fwdMostCommonBases);
        for (int j = 2; (i + j) < numBases; ++j) {
            if (fwdMostCommonBases[i + j] == nextBase) {
                ++hpCount;
            } else {
                break;
            }
        }

        // adjacent "homopolymer" is just one base, do not adjust deletion
        if (hpCount == 1) {
            return result;
        }

        // deletion is adjacent to a longer homopolymer
        // will fully ignore this position (high potential for messy HP boundaries)
        if (hpCount >= homopolymerThreshold) {
            result.HomopolymerAdjacent = true;
        }
        // deletion is adjacent to short hompolymer
        // will "squeeze" deletion, and use the adjacent base below
        else {
            result.UsingAdjacent = true;
            result.StrandToUpdate = Data::Strand::FORWARD;
        }
    }

    else if (revMostCommonBase == '-') {
        assert(fwdMostCommonBase != '-');

        // in the case of extreme deletion coverage, do not adjust
        // (even if adjacent to homopolymer)
        const auto revDelFraction = static_cast<double>(revMismatchCount) / revCoverageCount;
        const auto fwdMatchFraction =
            static_cast<double>(fwdCoverageCount - fwdMismatchCount) / fwdCoverageCount;
        if ((revDelFraction > 0.75) && (fwdMatchFraction < 0.25)) {
            return result;
        }

        // deletion is the most common "base", but not overwhelmingly so
        // scan ahead to see if we're adjacent to a homopolymer
        const char nextBase = revMostCommonBases[i + 1];
        int hpCount = 1;
        const int numBases = Utility::Ssize(revMostCommonBases);
        for (int j = 2; (i + j) < numBases; ++j) {
            if (revMostCommonBases[i + j] == nextBase) {
                ++hpCount;
            } else {
                break;
            }
        }

        // adjacent "homopolymer" is just one base, do not adjust deletion
        if (hpCount == 1) {
            return result;
        }

        // deletion is adjacent to a longer homopolymer
        // will fully ignore this position (high potential for messy HP boundaries)
        if (hpCount >= homopolymerThreshold) {
            result.HomopolymerAdjacent = true;
        }
        // deletion is adjacent to short hompolymer
        // will "squeeze" deletion, and use the adjacent base below
        else {
            result.UsingAdjacent = true;
            result.StrandToUpdate = Data::Strand::REVERSE;
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

    // gather pileup and potential mismatch sites from CIGARs
    auto fwdCigarCounts =
        internal::CigarMismatchCounts(reference, fwdSequences, fwdCigars, fwdPositions);
    auto revCigarCounts =
        internal::CigarMismatchCounts(reference, revSequences, revCigars, revPositions);

    const auto& fwdMismatchCounts = fwdCigarCounts.MismatchBaseCounts;
    const auto& revMismatchCounts = revCigarCounts.MismatchBaseCounts;
    const auto& fwdCoverageCounts = fwdCigarCounts.NumReads;
    const auto& revCoverageCounts = revCigarCounts.NumReads;
    auto& fwdMostCommonBases = fwdCigarCounts.MostCommonBases;
    auto& revMostCommonBases = revCigarCounts.MostCommonBases;
    const auto& fwdBaseCounts = fwdCigarCounts.BaseCounts;
    const auto& revBaseCounts = revCigarCounts.BaseCounts;

    assert(reference.size() == fwdMismatchCounts.size());
    assert(reference.size() == revMismatchCounts.size());
    assert(reference.size() == fwdCoverageCounts.size());
    assert(reference.size() == revCoverageCounts.size());
    assert(reference.size() == fwdMostCommonBases.size());
    assert(reference.size() == revMostCommonBases.size());
    assert(reference.size() == fwdBaseCounts.size());
    assert(reference.size() == revBaseCounts.size());

    std::vector<int32_t> variableSites;
    std::vector<int32_t> significantSites;
    std::vector<char> significantOperations;
    std::vector<char> significantBases;

    const int refLength = Utility::Ssize(reference);
    const int start = settings.IgnoreEndBases;
    const int end = refLength - settings.IgnoreEndBases;
    assert(end > start);

    for (int i = start; i < end; ++i) {

        // ensure most common bases are not same
        auto fwdMostCommonBase = fwdMostCommonBases[i];
        auto revMostCommonBase = revMostCommonBases[i];
        if (fwdMostCommonBase == revMostCommonBase) {
            continue;
        }
        const char refBase = reference[i];
        assert((fwdMostCommonBase != refBase) || (revMostCommonBase != refBase));

        // ensure some coverage on both strands
        auto fwdCoverageCount = fwdCoverageCounts[i];
        auto revCoverageCount = revCoverageCounts[i];
        if ((fwdCoverageCount == 0) || (revCoverageCount == 0)) {
            continue;
        }

        // ensure any mismatches on either strand
        auto fwdMismatchCount = fwdMismatchCounts[i];
        auto revMismatchCount = revMismatchCounts[i];
        if ((fwdMismatchCount == 0) && (revMismatchCount == 0)) {
            continue;
        }

        // PileupContext will determine if deletions are adjacent to hompolymers
        // or need to be "squeezed", in which case the next base should be used
        // at this position
        const auto pileupResult = internal::PileupContext(
            fwdMostCommonBase, fwdMostCommonBases, revMostCommonBase, revMostCommonBases,
            fwdCoverageCount, revCoverageCount, fwdMismatchCount, revMismatchCount, i, refLength,
            settings.HomopolymerThreshold);
        if (pileupResult.HomopolymerAdjacent) {
            continue;
        }
        if (pileupResult.StrandToUpdate == Data::Strand::FORWARD) {
            fwdMostCommonBase = revMostCommonBases[i + 1];
            fwdCoverageCount = internal::CoverageCount(fwdBaseCounts[i + 1]);
            fwdMismatchCount = internal::MismatchCount(fwdBaseCounts[i + 1], reference[i + 1]);
        } else if (pileupResult.StrandToUpdate == Data::Strand::REVERSE) {
            revMostCommonBase = revMostCommonBases[i + 1];
            revCoverageCount = internal::CoverageCount(revBaseCounts[i + 1]);
            revMismatchCount = internal::MismatchCount(revBaseCounts[i + 1], reference[i + 1]);
        } else {
            assert(pileupResult.StrandToUpdate == Data::Strand::UNMAPPED);
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

        // store all variable sites that have met coverage requirements
        variableSites.push_back(i);

        // determine the site's significance using Fisher's exact text
        const double pValue =
            CalculatePValue(fwdCoverageCount, fwdMismatchCount, revCoverageCount, revMismatchCount);
        if (pValue < settings.AlphaLevel) {
            significantSites.push_back(i);
            significantBases.push_back((fwdMostCommonBase == refBase) ? revMostCommonBase
                                                                      : fwdMostCommonBase);
        }

        // skip ahead an extra position, if we actually used the adjacent one here
        i += pileupResult.UsingAdjacent;
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
                               fwdMostCommonBases,
                               revMostCommonBases};
}

bool IsHeteroduplex(const std::string& reference, const std::vector<std::string>& fwdSequences,
                    const std::vector<std::string>& revSequences,
                    const std::vector<Data::Cigar>& fwdCigars,
                    const std::vector<Data::Cigar>& revCigars,
                    const std::vector<int32_t>& fwdPositions,
                    const std::vector<int32_t>& revPositions, const HeteroduplexSettings& settings)
{
    const auto results = FindHeteroduplex(reference, fwdSequences, revSequences, fwdCigars,
                                          revCigars, fwdPositions, revPositions, settings);
    return results.FractionSites >= settings.MinFractionSites;
}

}  // namespace Algorithm
}  // namespace PacBio
