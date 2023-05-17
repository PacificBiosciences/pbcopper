#include <pbcopper/data/Clipping.h>

#include <pbcopper/data/MappedRead.h>
#include <pbcopper/data/Read.h>
#include <pbcopper/data/internal/ClippingImpl.h>

#include <algorithm>

#include <cassert>

namespace PacBio {
namespace Data {
namespace {

template <typename T>
T clipContainer(const T& input, const std::size_t pos, const std::size_t len)
{
    if (input.empty()) {
        return {};
    }
    assert(input.size() >= pos + len);
    return T{input.cbegin() + pos, input.cbegin() + pos + len};
}

// returns reference positions removed from beginning
std::size_t ClipToQueryImpl(Cigar& cigar, std::size_t startOffset, std::size_t endOffset)
{
    std::size_t refPosRemoved = 0;
    std::size_t remaining = startOffset;

    // clip CIGAR ops from beginning of query sequence
    while ((remaining > 0) && !cigar.empty()) {
        CigarOperation& op = cigar.front();
        const auto opLength = op.Length();
        const bool consumesQuery = ConsumesQuery(op.Type());
        const bool consumesRef = ConsumesReference(op.Type());

        if (opLength <= remaining) {
            cigar.erase(cigar.begin());
            if (consumesQuery) {
                remaining -= opLength;
            }
            if (consumesRef) {
                refPosRemoved += opLength;
            }
        } else {
            op.Length(opLength - remaining);
            if (consumesRef) {
                refPosRemoved += remaining;
            }
            remaining = 0;
        }
    }

    // clip CIGAR ops from end of query sequence
    remaining = endOffset;
    while ((remaining > 0) && !cigar.empty()) {
        CigarOperation& op = cigar.back();
        const auto opLength = op.Length();
        const bool consumesQuery = ConsumesQuery(op.Type());

        if (opLength <= remaining) {
            cigar.pop_back();
            if (consumesQuery) {
                remaining -= opLength;
            }
        } else {
            op.Length(opLength - remaining);
            remaining = 0;
        }
    }

    return refPosRemoved;
}

void ClipToReferenceImpl(ClipToReferenceConfig& config, std::size_t* queryPosRemovedFront,
                         std::size_t* queryPosRemovedBack)
{

    const Position newTStart = std::max(config.original_tStart_, config.target_tStart_);
    const Position newTEnd = std::min(config.original_tEnd_, config.target_tEnd_);

    // fetch a 'working copy' of CIGAR data
    Cigar& cigar = config.cigar_;

    // ------------------------
    // clip leading CIGAR ops
    // ------------------------

    std::size_t remaining = newTStart - config.original_tStart_;
    while (remaining > 0 && !cigar.empty()) {
        CigarOperation& firstOp = cigar.front();
        const auto firstOpLength = firstOp.Length();
        const bool consumesQuery = ConsumesQuery(firstOp.Type());
        const bool consumesRef = ConsumesReference(firstOp.Type());

        if (!consumesRef) {

            // e.g. softclip - just pop it completely
            cigar.erase(cigar.begin());
            if (consumesQuery) {
                *queryPosRemovedFront += firstOpLength;
            }

        } else {
            assert(consumesRef);

            // CIGAR ends at or before clip
            if (firstOpLength <= remaining) {
                cigar.erase(cigar.begin());
                if (consumesQuery) {
                    *queryPosRemovedFront += firstOpLength;
                }
                if (consumesRef) {
                    remaining -= firstOpLength;
                }
            }

            // CIGAR straddles clip
            else {
                assert(firstOpLength > remaining);
                firstOp.Length(firstOpLength - remaining);
                if (consumesQuery) {
                    *queryPosRemovedFront += remaining;
                }
                remaining = 0;
            }
        }
    }

    // -------------------------
    // clip trailing CIGAR ops
    // -------------------------

    remaining = config.original_tEnd_ - newTEnd;
    while (remaining > 0 && !cigar.empty()) {
        CigarOperation& lastOp = cigar.back();
        const auto lastOpLength = lastOp.Length();
        const bool consumesQuery = ConsumesQuery(lastOp.Type());
        const bool consumesRef = ConsumesReference(lastOp.Type());

        if (!consumesRef) {

            // e.g. softclip - just pop it completely
            cigar.pop_back();
            if (consumesQuery) {
                *queryPosRemovedBack += lastOpLength;
            }

        } else {
            assert(consumesRef);

            // CIGAR ends at or after clip
            if (lastOpLength <= remaining) {
                cigar.pop_back();
                if (consumesQuery) {
                    *queryPosRemovedBack += lastOpLength;
                }
                if (consumesRef) {
                    remaining -= lastOpLength;
                }
            }

            // CIGAR straddles clip
            else {
                assert(lastOpLength > remaining);
                lastOp.Length(lastOpLength - remaining);
                if (consumesQuery) {
                    *queryPosRemovedBack += remaining;
                }
                remaining = 0;
            }
        }
    }

    if (config.exciseFlankingInserts_) {
        // check for leading insertion
        if (!cigar.empty()) {
            const CigarOperation& op = cigar.front();
            if (op.Type() == CigarOperationType::INSERTION) {
                *queryPosRemovedFront += op.Length();
                cigar.erase(cigar.begin());
            }
        }

        // check for trailing insertion
        if (!cigar.empty()) {
            const CigarOperation& op = cigar.back();
            if (op.Type() == CigarOperationType::INSERTION) {
                *queryPosRemovedBack += op.Length();
                cigar.pop_back();
            }
        }
    }
}

}  // namespace

namespace internal {

void ClipRead(Read& read, const ClipResult& result, std::size_t start, std::size_t end)
{
    const auto clipFrom = result.clipOffset_;
    const auto clipLength = (end - start);
    read.Seq = clipContainer(read.Seq, clipFrom, clipLength);
    read.Qualities = clipContainer(read.Qualities, clipFrom, clipLength);
    read.QueryStart = result.qStart_;
    read.QueryEnd = result.qEnd_;
    if (read.PulseWidth) {
        read.PulseWidth = clipContainer(read.PulseWidth->Data(), clipFrom, clipLength);
    }
    if (read.IPD) {
        read.IPD = clipContainer(read.IPD->Data(), clipFrom, clipLength);
    }
}

// NOTE: 'result' is moved into here, so we can take the CIGAR
void ClipMappedRead(MappedRead& read, ClipResult result)
{
    // clip common data
    ClipRead(read, result, result.qStart_, result.qEnd_);

    // clip mapped data
    read.Cigar = std::move(result.cigar_);
    read.TemplateStart = result.refPos_;
    read.TemplateEnd = read.TemplateStart + ReferenceLength(read.Cigar);
}

}  // namespace internal

ClipResult::ClipResult(std::size_t clipOffset, Position qStart, Position qEnd)
    : clipOffset_{clipOffset}, qStart_{qStart}, qEnd_{qEnd}
{}

ClipResult::ClipResult(std::size_t clipOffset, Position qStart, Position qEnd, Position refPos,
                       Cigar cigar)
    : clipOffset_{clipOffset}
    , qStart_{qStart}
    , qEnd_{qEnd}
    , refPos_{refPos}
    , cigar_{std::move(cigar)}
{}

ClipToReferenceConfig::ClipToReferenceConfig(const ClipToQueryConfig& queryConfig,
                                             Position originalTEnd, Position targetTStart,
                                             Position targetTEnd, bool exciseFlankingInserts)
    : ClipToQueryConfig(queryConfig)  // icc 17 hack
    , original_tEnd_{originalTEnd}
    , target_tStart_{targetTStart}
    , target_tEnd_{targetTEnd}
    , exciseFlankingInserts_{exciseFlankingInserts}
{}

ClipResult ClipToQuery(ClipToQueryConfig& config)
{
    // easy out for unmapped reads
    const std::size_t startOffset = (config.target_qStart_ - config.original_qStart_);
    if (!config.isMapped_) {
        return ClipResult{startOffset, config.target_qStart_, config.target_qEnd_};
    }

    // fetch CIGAR (in query orientation)
    Cigar cigar = std::move(config.cigar_);
    if (config.strand_ == Strand::REVERSE) {
        std::reverse(cigar.begin(), cigar.end());
    }

    // do main clipping
    const std::size_t endOffset = config.original_qEnd_ - config.target_qEnd_;
    const std::size_t refPosRemoved = ClipToQueryImpl(cigar, startOffset, endOffset);

    // maybe restore CIGAR
    if (config.strand_ == Strand::REVERSE) {
        std::reverse(cigar.begin(), cigar.end());
    }

    // return result
    const Position newPosition = (config.original_tStart_ + refPosRemoved);
    return ClipResult{startOffset, config.target_qStart_, config.target_qEnd_, newPosition,
                      std::move(cigar)};
}

ClipResult ClipToReference(ClipToReferenceConfig& config)
{
    assert(config.isMapped_);

    std::size_t queryPosRemovedFront = 0;
    std::size_t queryPosRemovedBack = 0;
    if (config.strand_ == Strand::FORWARD) {
        ClipToReferenceImpl(config, &queryPosRemovedFront, &queryPosRemovedBack);
    } else {
        ClipToReferenceImpl(config, &queryPosRemovedBack, &queryPosRemovedFront);
    }

    const std::size_t clipOffset = queryPosRemovedFront;
    const Position qStart = config.original_qStart_ + queryPosRemovedFront;
    const Position qEnd = config.original_qEnd_ - queryPosRemovedBack;
    const Position newPos = std::max(config.original_tStart_, config.target_tStart_);
    return ClipResult{clipOffset, qStart, qEnd, newPos, config.cigar_};
}

}  // namespace Data
}  // namespace PacBio
