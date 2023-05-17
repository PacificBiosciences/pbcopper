#include <pbcopper/align/ChainSeedsConfig.h>

namespace PacBio {
namespace Align {

ChainSeedsConfig::ChainSeedsConfig(const std::size_t numCandidatesArg, const long minScoreArg,
                                   const int matchScoreArg, const int nonMatchPenaltyArg,
                                   const int insertionPenaltyArg, const int deletionPenaltyArg,
                                   const int maxSeedGapArg)
    : numCandidates{numCandidatesArg}
    , minScore{minScoreArg}
    , matchScore{matchScoreArg}
    , nonMatchPenalty{nonMatchPenaltyArg}
    , insertionPenalty{insertionPenaltyArg}
    , deletionPenalty{deletionPenaltyArg}
    , maxSeedGap{maxSeedGapArg}
{}

ChainSeedsConfig::ChainSeedsConfig(const std::size_t numCandidatesArg)
    : numCandidates{numCandidatesArg}
{}

}  // namespace Align
}  // namespace PacBio
