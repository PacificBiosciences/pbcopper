#include <pbcopper/data/Frames.h>

#include <pbcopper/data/FrameEncoders.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <ostream>
#include <type_traits>

namespace PacBio {
namespace Data {

Frames::Frames(std::vector<std::uint16_t> frames) noexcept
    : std::vector<std::uint16_t>{std::move(frames)}
{}

const std::vector<std::uint16_t>& Frames::Data() const { return *this; }

std::vector<std::uint16_t>& Frames::Data() { return *this; }

std::vector<std::uint16_t>& Frames::DataRaw() { return *this; }

Frames Frames::Decode(const std::vector<std::uint8_t>& codedData)
{
    return Decode(codedData, V1FrameEncoder{});
}

std::vector<std::uint8_t> Frames::Encode(const std::vector<std::uint16_t>& frames)
{
    return Encode(frames, V1FrameEncoder{});
}

std::vector<std::uint8_t> Frames::Encode(FrameCodec /*unused*/) const { return Encode(*this); }

Frames& Frames::Data(std::vector<std::uint16_t> frames)
{
    *this = std::move(frames);
    return *this;
}

bool Frames::operator==(const Frames& other) const noexcept
{
    return static_cast<const std::vector<std::uint16_t>&>(*this) ==
           static_cast<const std::vector<std::uint16_t>&>(other);
}

bool Frames::operator!=(const Frames& other) const noexcept { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Frames& frames)
{
    return os << "Frames("
              << boost::algorithm::join(frames | boost::adaptors::transformed([](std::uint16_t i) {
                                            return std::to_string(i);
                                        }),
                                        ", ")
              << ')';
}

}  // namespace Data
}  // namespace PacBio
