#include <pbcopper/data/FrameEncoders.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <limits>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace PacBio {
namespace Data {
namespace {

static std::vector<uint16_t> framepoints;
static std::vector<uint8_t> frameToCode;
static uint16_t maxFramepoint;

static int InitIpdDownsampling()
{
    if (!framepoints.empty()) {
        return 0;
    }

    // liftover from kineticsTools

    const int B = 2;
    const int t = 6;
    const double T = std::pow(B, t);

    int next = 0;
    double grain;
    const int end = 256 / T;
    for (int i = 0; i < end; ++i) {
        grain = std::pow(B, i);
        std::vector<uint16_t> nextOnes;
        for (double j = 0; j < T; ++j) {
            nextOnes.push_back(j * grain + next);
        }
        next = nextOnes.back() + grain;
        framepoints.insert(framepoints.end(), nextOnes.cbegin(), nextOnes.cend());
    }
    assert(framepoints.size() - 1 <= std::numeric_limits<uint8_t>::max());

    const uint16_t maxElement = (*std::max_element(framepoints.cbegin(), framepoints.cend()));
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
            for (int f = fl; f < middle; ++f) {
                frameToCode[f] = i;
            }
            for (int f = middle; f < fu; ++f) {
                frameToCode[f] = i + 1;
            }
        } else {
            frameToCode[fl] = i;
        }
    }

    // this next line differs from the python implementation (there, it's "i+1")
    // our C++ for loop has incremented our index counter one more time than the indexes from python enumerate(...)
    frameToCode[fu] = i;
    maxFramepoint = fu;

    return 0;
}

static const int initV1Frames = InitIpdDownsampling();

}  // namespace

// ----------------
// V1 frame codec
// ----------------

Frames V1FrameEncoder::Decode(const std::vector<uint8_t>& encodedFrames) const
{
    std::vector<uint16_t> rawFrames;
    rawFrames.reserve(encodedFrames.size());
    std::transform(encodedFrames.cbegin(), encodedFrames.cend(), std::back_inserter(rawFrames),
                   [&](uint8_t code) { return framepoints[code]; });
    return rawFrames;
}

std::vector<uint8_t> V1FrameEncoder::Encode(const std::vector<uint16_t>& rawFrames) const
{
    std::vector<uint8_t> encoded;
    encoded.reserve(rawFrames.size());
    std::transform(rawFrames.cbegin(), rawFrames.cend(), std::back_inserter(encoded),
                   [&](uint16_t frame) { return frameToCode[std::min(maxFramepoint, frame)]; });
    return encoded;
}

std::string V1FrameEncoder::Name() const { return "CodecV1"; }

// ----------------
// V2 frame codec
// ----------------

V2FrameEncoder::V2FrameEncoder(int exponentBits, int mantissaBits)
    : exponentBits_{exponentBits}
    , mantissaBits_{mantissaBits}
    , base_(std::pow(2, mantissaBits_))
    , max_((1 << (exponentBits_ + mantissaBits_)) - 1)
{
}

Frames V2FrameEncoder::Decode(const std::vector<uint8_t>& encodedFrames) const
{
    std::vector<uint16_t> decoded;
    decoded.reserve(encodedFrames.size());
    std::transform(encodedFrames.cbegin(), encodedFrames.cend(), std::back_inserter(decoded),
                   [&](uint8_t x) -> uint16_t {
                       if (x > max_) {
                           throw std::runtime_error{"[pbcopper] invalid frame encoding ERROR: " +
                                                    std::to_string(x) + " is out of range [0," +
                                                    std::to_string(max_) + ']'};
                       }
                       const uint8_t mantissa = x & static_cast<uint8_t>((base_ - 1));
                       const uint8_t exponent = (x ^ mantissa) >> mantissaBits_;
                       return (base_ * (std::pow(2, exponent) - 1)) +
                              ((std::pow(2, exponent)) * mantissa);
                   });
    return Frames{decoded};
}

std::vector<uint8_t> V2FrameEncoder::Encode(const std::vector<uint16_t>& rawFrames) const
{
    // NOTE: nothing compressed here yet in output bytes, regardless of bitsPerPulse

    std::vector<uint8_t> encoded;
    encoded.reserve(rawFrames.size());
    std::transform(
        rawFrames.cbegin(), rawFrames.cend(), std::back_inserter(encoded), [&](uint16_t x) {
            const int exponent = std::log2(x / base_ + 1);
            const int mantissa =
                (x - base_ * (static_cast<uint8_t>(std::pow(2, exponent)) - 1)) >> exponent;
            const uint8_t result = (exponent << mantissaBits_) | mantissa;
            return std::min(result, max_);
        });
    return encoded;
}

int V2FrameEncoder::MantissaBits() const { return mantissaBits_; }

std::string V2FrameEncoder::Name() const
{
    return "CodecV2/" + std::to_string(exponentBits_) + '/' + std::to_string(mantissaBits_);
}

}  // namespace Data
}  // namespace PacBio
