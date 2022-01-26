#include <pbcopper/math/FishersExact.h>

#include <gtest/gtest.h>

using namespace PacBio;

//
// Tests pulled from htslib/kfunc
// https://github.com/samtools/htslib/blob/develop/test/test_kfunc.c
//
// NOTE: this assumes that htslib's own tests are correct
//
TEST(Math_FishersExact, can_calculate_pvalue)
{
    EXPECT_NEAR(Math::FishersExact(2, 1, 0, 31), 0.00534759, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(2, 1, 0, 1), 0.5, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(3, 1, 0, 0), 1.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(3, 15, 37, 45), 0.01713895, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(12, 5, 29, 2), 0.03907994, 1.0e+6);

    EXPECT_NEAR(Math::FishersExact(781, 23171, 4963, 2455001), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(333, 381, 801722, 7664285), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(4155, 4903, 805463, 8507517), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(4455, 4903, 805463, 8507517), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(5455, 4903, 805463, 8507517), 0.0, 1.0e+6);

    EXPECT_NEAR(Math::FishersExact(1, 1, 100000, 1000000), 0.165290623827, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(1000, 1000, 100000, 1000000), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(1000, 1000, 1000000, 100000), 0.0, 1.0e+6);

    EXPECT_NEAR(Math::FishersExact(49999, 10001, 90001, 49999), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(50000, 10000, 90000, 50000), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(50001, 9999, 89999, 50001), 0.0, 1.0e+6);
    EXPECT_NEAR(Math::FishersExact(10000, 50000, 130000, 10000), 0.0, 1.0e+6);
}
