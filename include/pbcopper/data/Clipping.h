#ifndef PBCOPPER_DATA_CLIPPING_H
#define PBCOPPER_DATA_CLIPPING_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Cigar.h>
#include <pbcopper/data/Position.h>
#include <pbcopper/data/Strand.h>

namespace PacBio {
namespace Data {

struct ClipToQueryConfig
{
    // all clipping
    size_t seqLength_;
    Position original_qStart_;
    Position original_qEnd_;
    Position target_qStart_;
    Position target_qEnd_;

    // for clipping mapped reads
    Position original_tStart_;
    Strand strand_;
    Cigar cigar_;
    bool isMapped_;
};

struct ClipToReferenceConfig : public ClipToQueryConfig
{
    ClipToReferenceConfig(const ClipToQueryConfig& queryConfig, Position originalTEnd,
                          Position targetTStart, Position targetTEnd, bool exciseFlankingInserts);

    Position original_tEnd_;
    Position target_tStart_;
    Position target_tEnd_;
    bool exciseFlankingInserts_;
};

struct ClipResult
{
    ClipResult(size_t clipOffset, Position qStart, Position qEnd);
    ClipResult(size_t clipOffset, Position qStart, Position qEnd, Position refPos, Cigar cigar);

    size_t clipOffset_;
    Position qStart_;
    Position qEnd_;
    Position refPos_;
    Cigar cigar_;
};

// configs are non-const so we can steal the input CIGAR, rather than copy,
// but they're otherwise const
ClipResult ClipToQuery(ClipToQueryConfig& config);
ClipResult ClipToReference(ClipToReferenceConfig& config);

}  // namespace BAM
}  // namespace PacBio

#endif  // PBCOPPER_DATA_CLIPPING_H
