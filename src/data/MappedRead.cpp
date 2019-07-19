// Author: Lance Hepler

#include <pbcopper/data/MappedRead.h>

#include <type_traits>

#include <pbcopper/data/Clipping.h>
#include <pbcopper/data/internal/ClippingImpl.h>

namespace PacBio {
namespace Data {

static_assert(std::is_copy_constructible<MappedRead>::value,
              "MappedRead(const MappedRead&) is not = default");
static_assert(std::is_copy_assignable<MappedRead>::value,
              "MappedRead& operator=(const MappedRead&) is not = default");

static_assert(std::is_nothrow_move_constructible<MappedRead>::value,
              "MappedRead(MappedRead&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<MappedRead>::value ==
                  std::is_nothrow_move_assignable<Read>::value,
              "");

MappedRead::MappedRead(const Read& read, enum Strand strand, size_t templateStart,
                       size_t templateEnd, bool pinStart, bool pinEnd)
    : Read(read)
    , Strand{strand}
    , TemplateStart(templateStart)
    , TemplateEnd(templateEnd)
    , PinStart{pinStart}
    , PinEnd{pinEnd}
{
}

MappedRead::MappedRead(const Read& read, PacBio::Data::Strand strand, Position templateStart,
                       Position templateEnd, PacBio::Data::Cigar cigar, uint8_t mapQV)
    : Read{read}
    , Strand{strand}
    , TemplateStart{templateStart}
    , TemplateEnd{templateEnd}
    , Cigar{std::move(cigar)}
    , MapQuality{mapQV}
{
}

std::ostream& operator<<(std::ostream& os, const MappedRead& mr)
{
    os << "MappedRead(Read(\"" << mr.Id << "\", \"" << mr.Seq << "\", \"" << mr.Model << "\"), ";
    switch (mr.Strand) {
        case Strand::FORWARD:
            os << "StrandType_FORWARD, ";
            break;
        case Strand::REVERSE:
            os << "StrandType_REVERSE, ";
            break;
        case Strand::UNMAPPED:
            os << "StrandType_UNMAPPED, ";
            break;
        default:
            throw std::runtime_error{"Unsupported Strand"};
    }
    os << mr.TemplateStart << ", " << mr.TemplateEnd << ", ";
    os << mr.PinStart << ", " << mr.PinEnd << ')';
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
