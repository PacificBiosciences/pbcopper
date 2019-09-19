// Author: Derek Barnett

#include <pbcopper/data/Frames.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <mutex>
#include <type_traits>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace PacBio {
namespace Data {
namespace {

static std::vector<uint16_t> framepoints;
static std::vector<uint8_t> frameToCode;
static uint16_t maxFramepoint;
static std::mutex initIpdDownsamplingMutex;

void InitIpdDownsampling()
{
    std::lock_guard<std::mutex> lock(initIpdDownsamplingMutex);

    if (!framepoints.empty()) return;

    // liftover from Dave's python code:
    // .../bioinformatics/tools/kineticsTools/kineticsTools/_downsampling.py

    const int B = 2;
    const int t = 6;
    const double T = pow(B, t);

    int next = 0;
    double grain;
    const int end = 256 / T;
    for (int i = 0; i < end; ++i) {
        grain = pow(B, i);
        std::vector<uint16_t> nextOnes;
        for (double j = 0; j < T; ++j)
            nextOnes.push_back(j * grain + next);
        next = nextOnes.back() + grain;
        framepoints.insert(framepoints.end(), nextOnes.cbegin(), nextOnes.cend());
    }
    assert(framepoints.size() - 1 <= std::numeric_limits<uint8_t>::max());

    const uint16_t maxElement = (*max_element(framepoints.cbegin(), framepoints.cend()));
    frameToCode.assign(maxElement + 1, 0);

    const int fpEnd = framepoints.size() - 1;
    uint8_t i = 0;
    uint16_t fl = 0;
    uint16_t fu = 0;
    for (; i < fpEnd; ++i) {
        fl = framepoints[i];
        fu = framepoints[i + 1];
        if (fu > fl + 1) {
            const int middle = (fl + fu) / 2;
            for (int f = fl; f < middle; ++f)
                frameToCode[f] = i;
            for (int f = middle; f < fu; ++f)
                frameToCode[f] = i + 1;
        } else
            frameToCode[fl] = i;
    }

    // this next line differs from the python implementation (there, it's "i+1")
    // our C++ for loop has incremented our index counter one more time than the indexes from python enumerate(...)
    frameToCode[fu] = i;
    maxFramepoint = fu;
}

uint16_t CodeToFrames(const uint8_t code) { return framepoints[code]; }

std::vector<uint16_t> CodeToFrames(const std::vector<uint8_t>& codedData)
{
    InitIpdDownsampling();

    const auto length = codedData.size();
    std::vector<uint16_t> frames(length, 0);
    for (size_t i = 0; i < length; ++i)
        frames[i] = CodeToFrames(codedData[i]);
    return frames;
}

uint8_t FramesToCode(const uint16_t frame) { return frameToCode[std::min(maxFramepoint, frame)]; }

std::vector<uint8_t> FramesToCode(const std::vector<uint16_t>& frames)
{
    InitIpdDownsampling();

    const auto length = frames.size();
    std::vector<uint8_t> result(length, 0);
    for (size_t i = 0; i < length; ++i)
        result[i] = FramesToCode(frames[i]);
    return result;
}

}  // namespace

static_assert(std::is_copy_constructible<Frames>::value, "Frames(const Frames&) is not = default");
static_assert(std::is_copy_assignable<Frames>::value,
              "Frames& operator=(const Frames&) is not = default");

static_assert(std::is_nothrow_move_constructible<Frames>::value,
              "Frames(Frames&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Frames>::value,
              "Frames& operator=(Frames&&) is not = noexcept");

Frames::Frames(std::vector<uint16_t> frames) noexcept : std::vector<uint16_t>{std::move(frames)} {}

const std::vector<uint16_t>& Frames::Data() const { return *this; }

std::vector<uint16_t>& Frames::DataRaw() { return *this; }

Frames Frames::Decode(const std::vector<uint8_t>& codedData) { return CodeToFrames(codedData); }

std::vector<uint8_t> Frames::Encode(const std::vector<uint16_t>& frames)
{
    return FramesToCode(frames);
}

std::vector<uint8_t> Frames::Encode() const { return Frames::Encode(*this); }

Frames& Frames::Data(std::vector<uint16_t> frames)
{
    *this = std::move(frames);
    return *this;
}

bool Frames::operator==(const Frames& other) const
{
    return static_cast<const std::vector<uint16_t>&>(*this) ==
           static_cast<const std::vector<uint16_t>&>(other);
}

bool Frames::operator!=(const Frames& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Frames& frames)
{
    return os << "Frames("
              << boost::algorithm::join(frames | boost::adaptors::transformed(
                                                     [](uint16_t i) { return std::to_string(i); }),
                                        ", ")
              << ')';
}

}  // namespace Data
}  // namespace PacBio
