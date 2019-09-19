// Author: Lance Hepler

#include <pbcopper/data/MappedRead.h>

#include <cassert>

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <type_traits>

#include <pbcopper/data/Clipping.h>
#include <pbcopper/data/internal/ClippingImpl.h>
#include <pbcopper/utility/SequenceUtils.h>

namespace PacBio {
namespace Data {
namespace {

struct InvalidMappedReadException : public std::runtime_error
{
    InvalidMappedReadException(std::string reason)
        : std::runtime_error{"[pbcopper] mapped read ERROR: " + reason}
    {
    }
};

template <class InputIt, class Size, class OutputIt>
OutputIt Move_N(InputIt first, Size count, OutputIt result)
{
    return std::move(first, first + count, result);
}

template <typename F, typename N>
void ClipAndGapify(F* seq, const Cigar& cigar, const GapBehavior gapBehavior,
                   const SoftClipBehavior softClipBehavior, N paddingNullValue, N deletionNullValue)
{
    assert(seq);
    assert((gapBehavior == GapBehavior::SHOW) || (softClipBehavior == SoftClipBehavior::REMOVE));

    const bool showGaps = (gapBehavior == GapBehavior::SHOW);
    const bool removeSoftClips = (softClipBehavior == SoftClipBehavior::REMOVE);

    // clang-format off
    // determine output container's length
    const size_t outputLength = std::accumulate(cigar.cbegin(), cigar.cend(), 0,
        [&](size_t totalLength, const CigarOperation& op) {
            const auto opLength = op.Length();
            switch (op.Type())
            {
                // these operations never increment output length
                case CigarOperationType::HARD_CLIP:
                case CigarOperationType::REFERENCE_SKIP:
                    return totalLength;

                // if we're removing soft clip, do not increment output length
                case CigarOperationType::SOFT_CLIP:
                    return totalLength + (removeSoftClips ? 0 : opLength);

                // increase output length only if we're adding deletion/padding chars
                case CigarOperationType::DELETION:
                case CigarOperationType::PADDING:
                    return totalLength + (showGaps ? opLength : 0);

                // otherwise, all operations contribute to output length
                default:
                    return totalLength + opLength;
            }
        });
    // clang-format on

    // move original data to temp, prep output container size
    F originalSeq = std::move(*seq);
    seq->resize(outputLength);

    // apply CIGAR ops
    size_t srcIndex = 0;
    size_t dstIndex = 0;
    for (const auto& op : cigar) {
        const auto opLength = op.Length();
        const auto opType = op.Type();

        // nothing to do for hard-clipped & ref-skipped positions
        if (opType == CigarOperationType::HARD_CLIP ||
            opType == CigarOperationType::REFERENCE_SKIP) {
            continue;
        }

        // maybe skip soft-clipped positions
        else if (opType == CigarOperationType::SOFT_CLIP && removeSoftClips)
            srcIndex += opLength;

        // maybe add deletions
        else if (opType == CigarOperationType::DELETION && showGaps) {
            for (size_t i = 0; i < opLength; ++i) {
                (*seq)[dstIndex] = deletionNullValue;
                ++dstIndex;
            }
        }

        // maybe add padding
        else if (opType == CigarOperationType::PADDING && showGaps) {
            for (size_t i = 0; i < opLength; ++i) {
                (*seq)[dstIndex] = paddingNullValue;
                ++dstIndex;
            }
        }

        // otherwise move input sequence to output
        else {
            Move_N(originalSeq.begin() + srcIndex, opLength, seq->begin() + dstIndex);
            srcIndex += opLength;
            dstIndex += opLength;
        }
    }
}

template <typename Container>
void OrientData(Container* data, Orientation currentOrientation, Orientation targetOrientation,
                enum Strand strand)
{
    if ((currentOrientation != targetOrientation) && (strand == Strand::REVERSE))
        std::reverse(data->begin(), data->end());
}

template <>
void OrientData(std::string* data, Orientation currentOrientation, Orientation targetOrientation,
                enum Strand strand)
{
    if ((currentOrientation != targetOrientation) && (strand == Strand::REVERSE))
        Utility::ReverseComplement(*data);
}

}  // namespace

static_assert(std::is_copy_constructible<MappedRead>::value,
              "MappedRead(const MappedRead&) is not = default");
static_assert(std::is_copy_assignable<MappedRead>::value,
              "MappedRead& operator=(const MappedRead&) is not = default");

static_assert(std::is_nothrow_move_constructible<MappedRead>::value,
              "MappedRead(MappedRead&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<MappedRead>::value ==
                  std::is_nothrow_move_assignable<Read>::value,
              "");

MappedRead::MappedRead(Read read) noexcept : Read{std::move(read)} {}

MappedRead::MappedRead(Read read, enum Strand strand, size_t templateStart, size_t templateEnd,
                       bool pinStart, bool pinEnd) noexcept
    : Read{std::move(read)}
    , Strand{strand}
    , TemplateStart(templateStart)
    , TemplateEnd(templateEnd)
    , PinStart{pinStart}
    , PinEnd{pinEnd}
{
}

MappedRead::MappedRead(Read read, PacBio::Data::Strand strand, Position templateStart,
                       PacBio::Data::Cigar cigar, uint8_t mapQV)
    : MappedRead{std::move(read), strand, templateStart, UnmappedPosition, std::move(cigar), mapQV}
{
    if (!Cigar.empty()) {
        TemplateEnd = TemplateStart;
        for (const auto& op : Cigar) {
            const auto type = op.Type();
            const auto len = op.Length();

            switch (type) {
                case CigarOperationType::ALIGNMENT_MATCH:
                case CigarOperationType::SEQUENCE_MATCH:
                case CigarOperationType::SEQUENCE_MISMATCH:
                case CigarOperationType::DELETION:
                case CigarOperationType::REFERENCE_SKIP:
                    TemplateEnd += len;
                    break;

                case CigarOperationType::INSERTION:
                case CigarOperationType::SOFT_CLIP:
                case CigarOperationType::HARD_CLIP:
                    break;

                default:
                    throw InvalidMappedReadException{"encountered unrecognized CIGAR operation"};
            }
        }
    }
}

MappedRead::MappedRead(Read read, PacBio::Data::Strand strand, Position templateStart,
                       Position templateEnd, PacBio::Data::Cigar cigar, uint8_t mapQV)
    : Read{std::move(read)}
    , Strand{strand}
    , TemplateStart{templateStart}
    , TemplateEnd{templateEnd}
    , Cigar{std::move(cigar)}
    , MapQuality{mapQV}
{
}

std::string MappedRead::AlignedSequence(Orientation orientation, GapBehavior gapBehavior,
                                        SoftClipBehavior softClipBehavior) const
{
    if (Strand == Strand::UNMAPPED || Cigar.empty()) return Seq;

    std::string bases = Seq;  // native orientation
    Orientation currentOrientation = Orientation::NATIVE;

    // if we need to touch CIGAR, force into genomic orientation (for mapping to CIGAR),
    // then apply gaps and/or remove soft clips
    if ((gapBehavior == GapBehavior::SHOW) || (softClipBehavior == SoftClipBehavior::REMOVE)) {
        OrientData(&bases, currentOrientation, Orientation::GENOMIC, AlignedStrand());
        currentOrientation = Orientation::GENOMIC;
        ClipAndGapify(&bases, Cigar, gapBehavior, softClipBehavior, '*', '-');
    }

    // place back in requested orientation
    OrientData(&bases, currentOrientation, orientation, AlignedStrand());
    return bases;
}

QualityValues MappedRead::AlignedQualities(Orientation orientation, GapBehavior gapBehavior,
                                           SoftClipBehavior softClipBehavior) const
{
    if (Strand == Strand::UNMAPPED || Cigar.empty()) return Qualities;

    QualityValues quals = Qualities;  // native orientation
    Orientation currentOrientation = Orientation::NATIVE;

    // if we need to touch CIGAR, force into genomic orientation (for mapping to CIGAR),
    // then apply gaps and/or remove soft clips
    if ((gapBehavior == GapBehavior::SHOW) || (softClipBehavior == SoftClipBehavior::REMOVE)) {
        OrientData(&quals, currentOrientation, Orientation::GENOMIC, AlignedStrand());
        currentOrientation = Orientation::GENOMIC;
        ClipAndGapify(&quals, Cigar, gapBehavior, softClipBehavior, QualityValue{0},
                      QualityValue{0});
    }

    // place back in requested orientation
    OrientData(&quals, currentOrientation, orientation, AlignedStrand());
    return quals;
}

boost::optional<Frames> MappedRead::AlignedIPD(Orientation orientation, GapBehavior gapBehavior,
                                               SoftClipBehavior softClipBehavior) const
{
    if (!IPD) return boost::none;
    if (Strand == Strand::UNMAPPED || Cigar.empty()) return IPD;

    Frames ipd = IPD.get();  // native orientation
    Orientation currentOrientation = Orientation::NATIVE;

    // if we need to touch CIGAR, force into genomic orientation (for mapping to CIGAR),
    // then apply gaps and/or remove soft clips
    if ((gapBehavior == GapBehavior::SHOW) || (softClipBehavior == SoftClipBehavior::REMOVE)) {
        OrientData(&ipd, currentOrientation, Orientation::GENOMIC, AlignedStrand());
        currentOrientation = Orientation::GENOMIC;
        ClipAndGapify(&ipd, Cigar, gapBehavior, softClipBehavior, 0, 0);
    }

    // place back in requested orientation
    OrientData(&ipd, currentOrientation, orientation, AlignedStrand());
    return ipd;
}

Frames MappedRead::AlignedPulseWidth(Orientation orientation, GapBehavior gapBehavior,
                                     SoftClipBehavior softClipBehavior) const
{
    if (Strand == Strand::UNMAPPED || Cigar.empty()) return PulseWidth;

    Frames pw = PulseWidth;  // native orientation
    Orientation currentOrientation = Orientation::NATIVE;

    // if we need to touch CIGAR, force into genomic orientation (for mapping to CIGAR),
    // then apply gaps and/or remove soft clips
    if ((gapBehavior == GapBehavior::SHOW) || (softClipBehavior == SoftClipBehavior::REMOVE)) {
        OrientData(&pw, currentOrientation, Orientation::GENOMIC, AlignedStrand());
        currentOrientation = Orientation::GENOMIC;
        ClipAndGapify(&pw, Cigar, gapBehavior, softClipBehavior, 0, 0);
    }

    // place back in requested orientation
    OrientData(&pw, currentOrientation, orientation, AlignedStrand());
    return pw;
}

Position MappedRead::AlignedStart() const
{
    if (QueryStart == UnmappedPosition)
        throw InvalidMappedReadException{"contains unmapped query start position"};
    if (Strand == Strand::UNMAPPED) throw InvalidMappedReadException{"contains unmapped strand"};

    Position startOffset = QueryStart;
    const Position seqLength = Seq.length();
    const auto findAlignedStart = [&startOffset, seqLength](auto it, const auto end) {
        for (; it != end; ++it) {
            const auto type = it->Type();
            const auto len = it->Length();
            if (type == CigarOperationType::HARD_CLIP) {
                if (startOffset != 0 && startOffset != seqLength) {
                    startOffset = -1;
                    break;
                }
            } else if (type == CigarOperationType::SOFT_CLIP)
                startOffset += len;
            else
                break;
        }
    };

    if (Strand == Strand::FORWARD)
        findAlignedStart(Cigar.cbegin(), Cigar.cend());
    else
        findAlignedStart(Cigar.crbegin(), Cigar.crend());

    return startOffset;
}

Position MappedRead::AlignedEnd() const
{
    if (QueryEnd == UnmappedPosition)
        throw InvalidMappedReadException{"contains unmapped query end position"};
    if (Strand == Strand::UNMAPPED) throw InvalidMappedReadException{"contains unmapped strand"};

    Position endOffset = QueryEnd;
    const Position seqLength = Seq.length();
    const auto findAlignedEnd = [&endOffset, seqLength](auto it, const auto end) {
        for (; it != end; ++it) {
            const auto type = it->Type();
            const auto len = it->Length();
            if (type == CigarOperationType::HARD_CLIP) {
                if (endOffset != 0 && endOffset != seqLength) {
                    endOffset = -1;
                    break;
                }
            } else if (type == CigarOperationType::SOFT_CLIP)
                endOffset -= len;
            else
                break;
        }
    };

    if (Strand == Strand::FORWARD)
        findAlignedEnd(Cigar.crbegin(), Cigar.crend());
    else
        findAlignedEnd(Cigar.cbegin(), Cigar.cend());

    return endOffset;
}

Position MappedRead::ReferenceStart() const
{
    if (TemplateStart == UnmappedPosition)
        throw InvalidMappedReadException{"contains unmapped template start position"};
    return TemplateStart;
}

Position MappedRead::ReferenceEnd() const
{
    if (TemplateEnd == UnmappedPosition)
        throw InvalidMappedReadException{"contains unmapped template end position"};
    return TemplateEnd;
}

Strand MappedRead::AlignedStrand() const { return Strand; }

size_t MappedRead::NumMismatches() const
{
    size_t result = 0;

    for (const auto& op : Cigar) {
        const auto type = op.Type();
        const auto len = op.Length();
        result += (type == CigarOperationType::SEQUENCE_MISMATCH) * len;
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const MappedRead& mr)
{
    os << "MappedRead(" << static_cast<const Read&>(mr) << ", RefId=" << mr.RefId << ", Strand=";
    switch (mr.Strand) {
        case Strand::FORWARD:
            os << "FORWARD";
            break;
        case Strand::REVERSE:
            os << "REVERSE";
            break;
        case Strand::UNMAPPED:
            os << "UNMAPPED";
            break;
        default:
            throw InvalidMappedReadException{"encountered unrecognized strand"};
    }
    os << ", TemplateStart=" << mr.TemplateStart << ", TemplateEnd=" << mr.TemplateEnd
       << ", PinStart=" << mr.PinStart << ", PinEnd=" << mr.PinEnd << ", Cigar=" << mr.Cigar
       << ", MapQuality=" << int{mr.MapQuality} << ')';
    return os;
}

void ClipToQuery(MappedRead& read, Position start, Position end)
{
    // skip out if clip not needed
    if (start <= read.QueryStart && end >= read.QueryEnd) return;

    // calculate clipping
    ClipToQueryConfig clipConfig{read.Seq.size(),
                                 read.QueryStart,
                                 read.QueryEnd,
                                 start,
                                 end,
                                 read.TemplateStart,
                                 read.Strand,
                                 std::move(read.Cigar),  // we'll get this back
                                 true};
    auto result = ClipToQuery(clipConfig);

    // apply clipping
    internal::ClipMappedRead(read, std::move(result));
}

void ClipToReference(MappedRead& read, Position start, Position end, bool exciseFlankingInserts)
{
    // return emptied read if clip region is disjoint from
    if (end <= read.TemplateStart || start >= read.TemplateEnd) {
        read.Seq.clear();
        read.Qualities.clear();
        read.QueryStart = -1;
        read.QueryEnd = -1;
        read.PulseWidth.clear();
        read.TemplateStart = -1;
        read.TemplateEnd = -1;
        read.Cigar.clear();
        read.MapQuality = 255;
        return;
    }

    // skip out if clip region covers aligned region (no clip needed)
    if (start <= read.TemplateStart && end >= read.TemplateEnd) return;

    // calculate clipping
    ClipToReferenceConfig clipConfig{
        ClipToQueryConfig{read.Seq.size(), read.QueryStart, read.QueryEnd, start, end,
                          read.TemplateStart, read.Strand,
                          std::move(read.Cigar),  // we'll get this back
                          true},
        read.TemplateEnd, start, end, exciseFlankingInserts};
    auto result = ClipToReference(clipConfig);

    // apply clipping
    internal::ClipMappedRead(read, std::move(result));
}

}  // namespace Data
}  // namespace PacBio
