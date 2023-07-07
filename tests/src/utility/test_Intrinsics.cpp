#include <pbcopper/utility/Intrinsics.h>
#include <pbcopper/utility/Random.h>

#include <gtest/gtest.h>

#include <array>
#include <limits>
#include <vector>

#include <cstddef>
#include <cstdint>

using namespace PacBio;

TEST(Utility_Intrinsics, popcount)
{
    for (int i = 0; i < 1000000; ++i) {
        EXPECT_EQ(Utility::PopCount(i), __builtin_popcount(i));
        EXPECT_EQ(Utility::PopCount(static_cast<unsigned>(i)), __builtin_popcount(i));
    }
    for (int i = 0; i < 64; ++i) {
        EXPECT_EQ(Utility::PopCount(std::uint64_t(1) << i), 1);
    }
}

TEST(Utility_Intrinsics, popcount_int8_t)
{
    constexpr std::array<std::int8_t, 7> INPUTS{0, 1, 2, 15, 16, -2, -1};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 4, 1, 7, 8};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_uint8_t)
{
    constexpr std::array<std::uint8_t, 7> INPUTS{0, 1, 2, 15, 16, 254, 255};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 4, 1, 7, 8};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_int16_t)
{
    constexpr std::array<std::int16_t, 7> INPUTS{0, 1, 2, 255, 256, -2, -1};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 8, 1, 15, 16};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_uint16_t)
{
    constexpr std::array<std::uint16_t, 7> INPUTS{0, 1, 2, 255, 256, 65534, 65535};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 8, 1, 15, 16};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_int32_t)
{
    constexpr std::array<std::int32_t, 7> INPUTS{0, 1, 2, 65535, 65536, -2, -1};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 16, 1, 31, 32};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_uint32_t)
{
    constexpr std::array<std::uint32_t, 7> INPUTS{0, 1, 2, 65535, 65536, 4294967294, 4294967295};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 16, 1, 31, 32};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_int64_t)
{
    constexpr std::array<std::int64_t, 7> INPUTS{0, 1, 2, 4294967295LL, 4294967296LL, -2, -1};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 32, 1, 63, 64};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_uint64_t)
{
    constexpr std::array<std::uint64_t, 7> INPUTS{
        0, 1, 2, 4294967295, 4294967296, 18446744073709551614ULL, 18446744073709551615ULL};
    constexpr std::array<int, 7> EXPECTED{0, 1, 1, 32, 1, 63, 64};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::PopCount(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, popcount_128)
{
    std::uint64_t seed = 137;
    __uint128_t v;
    for (int i = 0; i < 1000000; ++i) {
        const std::uint64_t hi = Utility::WyHash64Step(seed);
        const std::uint64_t lo = Utility::WyHash64Step(seed);
        v = (__uint128_t(hi) << 64) | lo;
        __int128_t signedV = v;
        EXPECT_EQ(Utility::PopCount(signedV), Utility::PopCount(hi) + Utility::PopCount(lo));
    }
    for (int i = 0; i < 128; ++i) {
        EXPECT_EQ(Utility::PopCount(__uint128_t(1) << i), 1);
        EXPECT_EQ(Utility::PopCount(__int128_t(1) << i), 1);
    }
}

TEST(Utility_Intrinsics, countlr)
{
    for (int i = 0; i < 64; ++i) {
        const std::uint64_t val = 1ull << i;
        EXPECT_EQ(Utility::CountTrailingZeros(val), i);
        EXPECT_EQ(63 - Utility::CountLeadingZeros(val), i);
    }
}

TEST(Utility_Intrinsics, countl_zero_int8_t)
{
    constexpr std::array<std::int8_t, 8> INPUTS{0, 1, 2, 15, 16, -128, -2, -1};
    constexpr std::array<int, 8> EXPECTED{8, 7, 6, 4, 3, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, countl_zero_uint8_t)
{
    constexpr std::array<std::uint8_t, 8> INPUTS{0, 1, 2, 15, 16, 128, 254, 255};
    constexpr std::array<int, 8> EXPECTED{8, 7, 6, 4, 3, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, countl_zero_int16_t)
{
    constexpr std::array<std::int16_t, 8> INPUTS{0, 1, 2, 15, 16, -32768, -2, -1};
    constexpr std::array<int, 8> EXPECTED{16, 15, 14, 12, 11, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, countl_zero_uint16_t)
{
    constexpr std::array<std::uint16_t, 8> INPUTS{0, 1, 2, 15, 16, 32768, 65534, 65535};
    constexpr std::array<int, 8> EXPECTED{16, 15, 14, 12, 11, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, countl_zero_int32_t)
{
    constexpr std::array<std::int32_t, 8> INPUTS{0, 1, 2, 15, 16, -2147483648, -2, -1};
    constexpr std::array<int, 8> EXPECTED{32, 31, 30, 28, 27, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, countl_zero_uint32_t)
{
    constexpr std::array<std::uint32_t, 8> INPUTS{0,  1,          2,          15,
                                                  16, 2147483648, 4294967294, 4294967295};
    constexpr std::array<int, 8> EXPECTED{32, 31, 30, 28, 27, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, countl_zero_int64_t)
{
    constexpr std::array<std::int64_t, 8> INPUTS{
        0, 1, 2, 15, 16, std::numeric_limits<std::int64_t>::min(), -2, -1};
    constexpr std::array<int, 8> EXPECTED{64, 63, 62, 60, 59, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, countl_zero_uint64_t)
{
    constexpr std::array<std::uint64_t, 8> INPUTS{
        0, 1, 2, 15, 16, 9223372036854775808ULL, 18446744073709551614ULL, 18446744073709551615ULL};
    constexpr std::array<int, 8> EXPECTED{64, 63, 62, 60, 59, 0, 0, 0};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::CountLeadingZeros(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_int8_t)
{
    constexpr std::array<std::int8_t, 5> INPUTS{1, 2, 15, 16, 127};
    constexpr std::array<int, 5> EXPECTED{0, 1, 3, 4, 6};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_uint8_t)
{
    constexpr std::array<std::uint8_t, 6> INPUTS{1, 2, 15, 16, 127, 255};
    constexpr std::array<int, 6> EXPECTED{0, 1, 3, 4, 6, 7};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_int16_t)
{
    constexpr std::array<std::int16_t, 5> INPUTS{1, 2, 127, 128, 32767};
    constexpr std::array<int, 5> EXPECTED{0, 1, 6, 7, 14};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_uint16_t)
{
    constexpr std::array<std::uint16_t, 6> INPUTS{1, 2, 127, 128, 32767, 65535};
    constexpr std::array<int, 6> EXPECTED{0, 1, 6, 7, 14, 15};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_int32_t)
{
    constexpr std::array<std::int32_t, 5> INPUTS{1, 2, 32767, 32768, 2147483647};
    constexpr std::array<int, 5> EXPECTED{0, 1, 14, 15, 30};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_uint32_t)
{
    constexpr std::array<std::uint32_t, 6> INPUTS{1, 2, 32767, 32768, 2147483647, 4294967295};
    constexpr std::array<int, 6> EXPECTED{0, 1, 14, 15, 30, 31};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_int64_t)
{
    constexpr std::array<std::int64_t, 5> INPUTS{1, 2, 2147483647, 2147483648,
                                                 9223372036854775807LL};
    constexpr std::array<int, 5> EXPECTED{0, 1, 30, 31, 62};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}

TEST(Utility_Intrinsics, IntegralLog2_uint64_t)
{
    constexpr std::array<std::uint64_t, 6> INPUTS{
        1, 2, 2147483647, 2147483648, 9223372036854775807ULL, 18446744073709551615ULL};
    constexpr std::array<int, 6> EXPECTED{0, 1, 30, 31, 62, 63};

    for (std::size_t i = 0; i < INPUTS.size(); ++i) {
        EXPECT_EQ(Utility::IntegralLog2(INPUTS[i]), EXPECTED[i]);
    }
}
