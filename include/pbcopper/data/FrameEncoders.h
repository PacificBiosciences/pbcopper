// Author: Derek Barnett

#ifndef PBCOPPER_DATA_FRAMEENCODERS_H
#define PBCOPPER_DATA_FRAMEENCODERS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Frames.h>

namespace PacBio {
namespace Data {

class V1FrameEncoder
{
public:
    V1FrameEncoder() = default;

    std::vector<uint8_t> Encode(const std::vector<uint16_t>& rawFrames) const;
    Frames Decode(const std::vector<uint8_t>& encodedFrames) const;
};

struct V2FrameEncoder
{
public:
    V2FrameEncoder(int mantissaBits);

    std::vector<uint8_t> Encode(const std::vector<uint16_t>& rawFrames) const;
    Frames Decode(const std::vector<uint8_t>& encodedFrames) const;

private:
    int mantissaBits_;
    int base_;
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_FRAMEENCODERS_H
