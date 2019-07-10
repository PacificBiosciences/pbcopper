// Author: Derek Barnett

#include <pbcopper/data/SimpleRead.h>

#include <type_traits>

#include <pbcopper/data/Clipping.h>
#include <pbcopper/data/internal/ClippingImpl.h>

namespace PacBio {
namespace Data {

static_assert(std::is_copy_constructible<SimpleRead>::value,
              "SimpleRead(const SimpleRead&) is not = default");
static_assert(std::is_copy_assignable<SimpleRead>::value,
              "SimpleRead& operator=(const SimpleRead&) is not = default");

static_assert(std::is_nothrow_move_constructible<SimpleRead>::value,
              "SimpleRead(SimpleRead&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<SimpleRead>::value ==
                  std::is_nothrow_move_assignable<std::string>::value,
              "");

SimpleRead::SimpleRead(std::string name, std::string seq, QualityValues qualities, SNR snr)
    : Name{std::move(name)}
    , Sequence{std::move(seq)}
    , Qualities{std::move(qualities)}
    , SignalToNoise{std::move(snr)}
{
    QueryStart = 0;
    QueryEnd = Sequence.size();
}

SimpleRead::SimpleRead(std::string name, std::string seq, QualityValues qualities, SNR snr,
                       Position qStart, Position qEnd)
    : Name{std::move(name)}
    , Sequence{std::move(seq)}
    , Qualities{std::move(qualities)}
    , SignalToNoise{std::move(snr)}
    , QueryStart{qStart}
    , QueryEnd{qEnd}
{
}

SimpleRead::SimpleRead(std::string name, std::string seq, QualityValues qualities, SNR snr,
                       Position qStart, Position qEnd, Frames pulseWidths, Frames ipd)
    : Name{std::move(name)}
    , Sequence{std::move(seq)}
    , Qualities{std::move(qualities)}
    , SignalToNoise{std::move(snr)}
    , QueryStart{qStart}
    , QueryEnd{qEnd}
    , PulseWidths{std::move(pulseWidths)}
    , IPD{std::move(ipd)}
{
}

void ClipToQuery(SimpleRead& read, Position start, Position end)
{
    // skip out if clip not needed
    if (start <= read.QueryStart && end >= read.QueryEnd) return;

    // calculate clipping
    ClipToQueryConfig clipConfig{read.Sequence.size(),
                                 read.QueryStart,
                                 read.QueryEnd,
                                 start,
                                 end,
                                 // TODO: these next are unused for unmapped clip-to-query
                                 UnmappedPosition,
                                 Strand::FORWARD,
                                 {},
                                 false};
    auto result = ClipToQuery(clipConfig);

    // apply clipping
    internal::ClipSimpleRead(read, std::move(result), start, end);
}

}  // namespace Data
}  // namespace PacBio
