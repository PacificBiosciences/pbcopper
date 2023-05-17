#ifndef PBCOPPER_DATA_MAPPEDREAD_H
#define PBCOPPER_DATA_MAPPEDREAD_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Cigar.h>
#include <pbcopper/data/Orientation.h>
#include <pbcopper/data/Read.h>
#include <pbcopper/data/Strand.h>

#include <iosfwd>

namespace PacBio {
namespace Data {

enum class GapBehavior
{
    IGNORE,
    SHOW
};

enum class SoftClipBehavior
{
    KEEP,
    REMOVE
};

/// A MappedRead extends Read by the strand information and template anchoring
/// positions.
struct MappedRead : public Read
{
    MappedRead(Read read) noexcept;
    MappedRead(Read read, enum Strand strand, Position templateStart, Position templateEnd,
               bool pinStart = false, bool pinEnd = false) noexcept;
    MappedRead(Read read, enum Strand strand, Position templateStart, Cigar cigar,
               std::uint8_t mapQV);
    MappedRead(Read read, enum Strand strand, Position templateStart, Position templateEnd,
               Cigar cigar, std::uint8_t mapQV);

    std::int32_t RefId = 0;
    enum Strand Strand = Strand::UNMAPPED;
    Position TemplateStart = UNMAPPED_POSITION;
    Position TemplateEnd = UNMAPPED_POSITION;
    bool PinStart = false;
    bool PinEnd = false;
    class Cigar Cigar;
    std::uint8_t MapQuality = 0;

    Position AlignedStart() const;
    Position AlignedEnd() const;
    enum Strand AlignedStrand() const;
    Position ReferenceStart() const;
    Position ReferenceEnd() const;

    std::string AlignedSequence(Orientation orientation = Orientation::NATIVE,
                                GapBehavior gapBehavior = GapBehavior::IGNORE,
                                SoftClipBehavior softClipBehavior = SoftClipBehavior::KEEP) const;

    QualityValues AlignedQualities(
        Orientation orientation = Orientation::NATIVE,
        GapBehavior gapBehavior = GapBehavior::IGNORE,
        SoftClipBehavior softClipBehavior = SoftClipBehavior::KEEP) const;

    std::optional<Frames> AlignedIPD(
        Orientation orientation = Orientation::NATIVE,
        GapBehavior gapBehavior = GapBehavior::IGNORE,
        SoftClipBehavior softClipBehavior = SoftClipBehavior::KEEP) const;

    std::optional<Frames> AlignedPulseWidth(
        Orientation orientation = Orientation::NATIVE,
        GapBehavior gapBehavior = GapBehavior::IGNORE,
        SoftClipBehavior softClipBehavior = SoftClipBehavior::KEEP) const;

    std::int32_t NumMismatches() const;
};

///
/// \brief
///
/// \param read
/// \param start
/// \param end
///
void ClipToQuery(MappedRead& read, Position start, Position end);

///
/// \brief
///
/// \param read
/// \param start
/// \param end
/// \param exciseFlankingInserts
///
void ClipToReference(MappedRead& read, Position start, Position end, bool exciseFlankingInserts);

std::ostream& operator<<(std::ostream&, const MappedRead&);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_MAPPEDREAD_H
