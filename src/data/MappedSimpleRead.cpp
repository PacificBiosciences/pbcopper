// Author: Derek Barnett

#include <pbcopper/data/MappedSimpleRead.h>

#include <type_traits>

#include <pbcopper/data/Clipping.h>
#include <pbcopper/data/internal/ClippingImpl.h>

namespace PacBio {
namespace Data {

static_assert(std::is_copy_constructible<MappedSimpleRead>::value,
              "MappedSimpleRead(const MappedSimpleRead&) is not = default");
static_assert(std::is_copy_assignable<MappedSimpleRead>::value,
              "MappedSimpleRead& operator=(const MappedSimpleRead&) is not = default");

static_assert(std::is_nothrow_move_constructible<MappedSimpleRead>::value,
              "MappedSimpleRead(MappedSimpleRead&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<MappedSimpleRead>::value ==
                  std::is_nothrow_move_assignable<SimpleRead>::value,
              "");

MappedSimpleRead::MappedSimpleRead(const SimpleRead& read, PacBio::Data::Strand strand,
                                   Position templateStart, Position templateEnd,
                                   PacBio::Data::Cigar cigar, uint8_t mapQV)
    : SimpleRead{read}
    , Strand{strand}
    , TemplateStart{templateStart}
    , TemplateEnd{templateEnd}
    , Cigar{std::move(cigar)}
    , MapQuality{mapQV}
{
}

void ClipToQuery(MappedSimpleRead& read, Position start, Position end)
{
    // skip out if clip not needed
    if (start <= read.QueryStart && end >= read.QueryEnd) return;

    // calculate clipping
    ClipToQueryConfig clipConfig{read.Sequence.size(),
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

void ClipToReference(MappedSimpleRead& read, Position start, Position end,
                     bool exciseFlankingInserts)
{
    // return emptied read if clip region is disjoint from
    if (end <= read.TemplateStart || start >= read.TemplateEnd) {
        read.Sequence.clear();
        read.Qualities.clear();
        read.QueryStart = -1;
        read.QueryEnd = -1;
        if (read.PulseWidths) read.PulseWidths->DataRaw().clear();
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
        ClipToQueryConfig{read.Sequence.size(), read.QueryStart, read.QueryEnd, start, end,
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