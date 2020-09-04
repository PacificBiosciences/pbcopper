// Author: Derek Barnett

#ifndef PBCOPPER_DATA_FRAMEENCODERS_H
#define PBCOPPER_DATA_FRAMEENCODERS_H

#include <pbcopper/PbcopperConfig.h>

#include <memory>

#include <pbcopper/data/Frames.h>

namespace PacBio {
namespace Data {

// --------------
// Codecs
// --------------

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

// ---------------------
// Type-erased encoder
// ---------------------

class FrameEncoder
{
public:
    template <typename T>
    FrameEncoder(T codec) : self_{std::make_unique<EncoderImpl<T>>(std::move(codec))}
    {
    }
    FrameEncoder(const FrameEncoder& other) : self_{other.self_->Clone()} {}
    FrameEncoder(FrameEncoder&&) noexcept = default;
    FrameEncoder& operator=(const FrameEncoder& other)
    {
        self_.reset(other.self_->Clone());
        return *this;
    }
    FrameEncoder& operator=(FrameEncoder&&) noexcept = default;

public:
    std::vector<uint8_t> Encode(const std::vector<uint16_t>& rawFrames) const
    {
        return self_->Encode(rawFrames);
    }

    Frames Decode(const std::vector<uint8_t>& encodedFrames) const
    {
        return self_->Decode(encodedFrames);
    }

private:
    struct EncoderInterface
    {
        virtual ~EncoderInterface() = default;
        virtual EncoderInterface* Clone() const = 0;
        virtual std::vector<uint8_t> Encode(const std::vector<uint16_t>& rawFrames) const = 0;
        virtual Frames Decode(const std::vector<uint8_t>& encodedFrames) const = 0;
    };

    template <typename T>
    struct EncoderImpl : public EncoderInterface
    {
        EncoderImpl(T codec) : EncoderInterface{}, codec_(std::move(codec)) {}
        EncoderImpl(const EncoderImpl& other) : EncoderInterface{}, codec_(other.codec_) {}
        EncoderImpl* Clone() const override { return new EncoderImpl(*this); }
        std::vector<uint8_t> Encode(const std::vector<uint16_t>& rawFrames) const override
        {
            return codec_.Encode(rawFrames);
        }
        Frames Decode(const std::vector<uint8_t>& encodedFrames) const override
        {
            return codec_.Decode(encodedFrames);
        }
        T codec_;
    };

    std::unique_ptr<EncoderInterface> self_;
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_FRAMEENCODERS_H
