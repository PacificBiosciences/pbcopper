#include <pbcopper/data/FrameEncoders.h>
#include <pbcopper/data/Frames.h>

#include <cstdint>

#include <numeric>
#include <vector>

#include <gtest/gtest.h>

using Frames = PacBio::Data::Frames;

// clang-format off
namespace FramesTests {

const std::vector<uint16_t> RawFrames{
    0,  8,  140, 0,  0,   7,  4,  0,  85, 2,  1,  3,  2,   10, 1,  20, 47,   10,  9,  60, 20,
    3,  12, 5,   13, 165, 6,  14, 22, 12, 2,  4,  9,  218, 27, 3,  15, 2,    17,  2,  45, 24,
    89, 10, 7,   1,  11,  15, 0,  7,  0,  28, 17, 12, 6,   10, 37, 0,  12,   52,  0,  7,  1,
    14, 3,  26,  12, 0,   20, 17, 2,  13, 2,  9,  13, 7,   15, 29, 3,  6,    2,   1,  28, 10,
    3,  14, 7,   1,  22,  1,  6,  6,  0,  19, 31, 6,  2,   14, 0,  0,  1000, 947, 948};

const std::vector<uint8_t> EncodedFrames{
    0,  8,  102, 0,  0,   7,  4,  0,  75, 2,  1,  3,  2,   10, 1,  20, 47,  10,  9,  60, 20,
    3,  12, 5,   13, 115, 6,  14, 22, 12, 2,  4,  9,  135, 27, 3,  15, 2,   17,  2,  45, 24,
    77, 10, 7,   1,  11,  15, 0,  7,  0,  28, 17, 12, 6,   10, 37, 0,  12,  52,  0,  7,  1,
    14, 3,  26,  12, 0,   20, 17, 2,  13, 2,  9,  13, 7,   15, 29, 3,  6,   2,   1,  28, 10,
    3,  14, 7,   1,  22,  1,  6,  6,  0,  19, 31, 6,  2,   14, 0,  0,  255, 254, 255};

}  // namespace FramesTests
// clang-format on

TEST(Data_Frames, default_is_empty)
{
    const Frames f;
    ASSERT_TRUE(f.Data().empty());
}

TEST(Data_Frames, can_construct_from_raw_frames)
{
    const Frames f{FramesTests::RawFrames};
    const auto d = f.Data();
    ASSERT_EQ(FramesTests::RawFrames, d);
}

TEST(Data_Frames, can_encode_frames_from_raw_data)
{
    const Frames f{FramesTests::RawFrames};
    const auto e = f.Encode();
    ASSERT_EQ(FramesTests::EncodedFrames, e);
}

TEST(Data_FrameEncoder, can_default_initialize)
{
    // make sure icpc (legacy) compilation works with this construct... sigh...
    struct TestInitialize
    {
        PacBio::Data::FrameEncoder v1 = PacBio::Data::V1FrameEncoder{};
        PacBio::Data::FrameEncoder v2 = PacBio::Data::V1FrameEncoder{};
    };
    EXPECT_TRUE(true);
}

TEST(Data_FrameEncoder, decode_throws_if_out_of_range)
{
    PacBio::Data::V2FrameEncoder v2{3, 3};
    EXPECT_THROW({ auto x = v2.Decode({64}); }, std::runtime_error);
    EXPECT_NO_THROW({ auto x = v2.Decode({0}); });
    EXPECT_NO_THROW({ auto x = v2.Decode({1}); });
    EXPECT_NO_THROW({ auto x = v2.Decode({63}); });
}

TEST(Data_FrameEncoder, v2_encoder_produces_expected_values)
{
    std::vector<uint16_t> input(240);
    std::iota(input.begin(), input.end(), 0);

    const std::vector<uint8_t> expectedEncoded{
        // 0 - 15
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        // 16 - 31
        16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23,
        // 32 - 47
        24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31,
        // 48 - 63
        32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35,
        // 64 - 79
        36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39,
        // 80 - 95
        40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43,
        // 96 - 111
        44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47,
        // 112 - 127
        48, 48, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49,
        // 128 - 143
        50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 51, 51,
        // 144 - 159
        52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 53, 53,
        // 160 - 175
        54, 54, 54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55,
        // 176 - 191
        56, 56, 56, 56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 57, 57, 57,
        // 192 - 207
        58, 58, 58, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 59, 59, 59,
        // 208 - 223
        60, 60, 60, 60, 60, 60, 60, 60, 61, 61, 61, 61, 61, 61, 61, 61,
        // 224 - 239
        62, 62, 62, 62, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63,
    };

    PacBio::Data::V2FrameEncoder v2{2, 4};
    const auto encoded = v2.Encode(input);
    EXPECT_EQ(encoded.size(), expectedEncoded.size());
    EXPECT_EQ(encoded, expectedEncoded);
}

TEST(Data_FrameEncoder, v2_encoder_clamps_to_max)
{
    const std::vector<uint16_t> outOfRange{240, 256, 400};
    const std::vector<uint8_t> clamped{63, 63, 63};

    PacBio::Data::V2FrameEncoder v2{2, 4};
    const auto encoded = v2.Encode(outOfRange);
    EXPECT_EQ(clamped, encoded);
}

TEST(Data_FrameEncoder, v2_decoder_produces_expected_values)
{
    std::vector<uint8_t> input(64);
    std::iota(input.begin(), input.end(), 0);

    const std::vector<uint16_t> expectedDecoded{
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
        16,  18,  20,  22,  24,  26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,
        48,  52,  56,  60,  64,  68,  72,  76,  80,  84,  88,  92,  96,  100, 104, 108,
        112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232,
    };

    PacBio::Data::V2FrameEncoder v2{2, 4};
    const auto decoded = v2.Decode(input);
    EXPECT_EQ(decoded.Data(), expectedDecoded);

    // out-of-range
    std::vector<uint8_t> outOfRange{64, 65, 66};
    EXPECT_THROW(v2.Decode(outOfRange), std::runtime_error);
}
