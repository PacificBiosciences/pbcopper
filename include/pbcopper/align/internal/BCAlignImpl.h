// Author: Derek Barnett

//
// BandedChainAlignment implementation
//

#ifndef PBCOPPER_ALIGN_BCALIGNIMPL_H
#define PBCOPPER_ALIGN_BCALIGNIMPL_H

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>

#include <pbcopper/align/BandedChainAlignment.h>
#include <pbcopper/align/internal/BCAlignBlocks.h>

namespace PacBio {
namespace Align {
namespace Internal {

class BandedChainAlignerImpl
{
public:
    BandedChainAlignerImpl(const BandedChainAlignConfig& config);

public:
    BandedChainAlignment Align(const char* target, const size_t targetLen, const char* query,
                               const size_t queryLen,
                               const std::vector<PacBio::Align::Seed>& seeds);

    void StitchTranscripts(std::string* global, std::string&& local);

private:
    struct Sequences
    {
        const char* target;
        size_t targetLen;
        const char* query;
        size_t queryLen;
    };

    void AlignGapBlock(const PacBio::Align::Seed& nextSeed);
    void AlignGapBlock(const size_t hLength, const size_t vLength);
    void AlignLastGapBlock(void);

    void AlignSeedBlock(const PacBio::Align::Seed& seed);

    void Initialize(const char* target, const size_t targetLen, const char* query,
                    const size_t queryLen);

    std::vector<PacBio::Align::Seed> MergeSeeds(const std::vector<PacBio::Align::Seed>& seeds);

    BandedChainAlignment Result(void);

private:
    const BandedChainAlignConfig& config_;

    StandardGlobalAlignBlock gapBlock_;
    BandedGlobalAlignBlock seedBlock_;
    std::string globalTranscript_;
    int64_t globalScore_;
    size_t gapBlockBeginH_;
    size_t gapBlockBeginV_;
    Sequences sequences_;
};

}  // namespace Internal
}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_BCALIGNIMPL_H
