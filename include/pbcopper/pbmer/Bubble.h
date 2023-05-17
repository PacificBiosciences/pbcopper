#ifndef PBCOPPER_PBMER_BUBBLE_H
#define PBCOPPER_PBMER_BUBBLE_H

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/pbmer/DnaBit.h>

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <cstdint>

namespace PacBio {
namespace Pbmer {

// simple structure that returns the bubble sequences and readIds across the paths
struct BubbleInfo
{
    // sequence of left path
    std::string LSeq;
    // sequence of right path
    std::string RSeq;
    // count of kmers along left paths
    std::int32_t LKmerCount;
    // count of kmers along right path
    std::int32_t RKmerCount;

    // READID, KMER_COUNT
    std::vector<std::pair<std::uint32_t, std::int32_t>> LData;
    std::vector<std::pair<std::uint32_t, std::int32_t>> RData;
    // K-mers from the left-hand side
    std::vector<DnaBit> LVec;
    // K-mers from the right-hand side
    std::vector<DnaBit> RVec;
};

using Bubbles = std::vector<BubbleInfo>;

}  // namespace Pbmer
}  // namespace PacBio
#endif  // PBCOPPER_PBMER_BUBBLE_H
