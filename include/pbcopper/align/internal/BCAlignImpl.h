#ifndef PBCOPPER_ALIGN_BCALIGNIMPL_H
#define PBCOPPER_ALIGN_BCALIGNIMPL_H

//
// BandedChainAlignment implementation
//

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/BandedChainAlignment.h>
#include <pbcopper/align/internal/BCAlignBlocks.h>

#include <string>
#include <vector>

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace Align {
namespace Internal {

class BandedChainAlignerImpl
{
public:
    BandedChainAlignerImpl(const BandedChainAlignConfig& config);

public:
    BandedChainAlignment Align(const char* target, std::size_t targetLen, const char* query,
                               std::size_t queryLen, const std::vector<PacBio::Align::Seed>& seeds);

    void StitchTranscripts(std::string* global, std::string&& local);

private:
    struct Sequences
    {
        const char* target;
        std::size_t targetLen;
        const char* query;
        std::size_t queryLen;
    };

    void AlignGapBlock(const PacBio::Align::Seed& nextSeed);
    void AlignGapBlock(std::size_t hLength, std::size_t vLength);
    void AlignLastGapBlock();

    void AlignSeedBlock(const PacBio::Align::Seed& seed);

    void Initialize(const char* target, std::size_t targetLen, const char* query,
                    std::size_t queryLen);

    std::vector<PacBio::Align::Seed> MergeSeeds(const std::vector<PacBio::Align::Seed>& seeds);

    BandedChainAlignment Result();

private:
    const BandedChainAlignConfig& config_;

    StandardGlobalAlignBlock gapBlock_;
    BandedGlobalAlignBlock seedBlock_;
    std::string globalTranscript_;
    std::int64_t globalScore_;
    std::size_t gapBlockBeginH_;
    std::size_t gapBlockBeginV_;
    Sequences sequences_;
};

}  // namespace Internal
}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_BCALIGNIMPL_H
