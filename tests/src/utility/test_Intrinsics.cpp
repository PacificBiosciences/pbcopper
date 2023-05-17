#include <pbcopper/utility/Intrinsics.h>
#include <pbcopper/utility/Random.h>

#include <vector>

#include <gtest/gtest.h>

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
