#include <pbcopper/numeric/Helper.h>

#include <gtest/gtest.h>

TEST(Numeric, IsPowerOfTwo_test)
{
    // powers of two
    EXPECT_TRUE(PacBio::Numeric::IsPowerOfTwo(1));
    EXPECT_TRUE(PacBio::Numeric::IsPowerOfTwo(4));
    EXPECT_TRUE(PacBio::Numeric::IsPowerOfTwo(8));
    EXPECT_TRUE(PacBio::Numeric::IsPowerOfTwo(1024));
    EXPECT_TRUE(PacBio::Numeric::IsPowerOfTwo(4096));

    // non-powers of two
    EXPECT_FALSE(PacBio::Numeric::IsPowerOfTwo(0));
    EXPECT_FALSE(PacBio::Numeric::IsPowerOfTwo(3));
    EXPECT_FALSE(PacBio::Numeric::IsPowerOfTwo(5));
    EXPECT_FALSE(PacBio::Numeric::IsPowerOfTwo(1023));
    EXPECT_FALSE(PacBio::Numeric::IsPowerOfTwo(4098));
}

TEST(Numeric, NextPowerOfTwo_test)
{
    EXPECT_EQ(PacBio::Numeric::NextPowerOfTwo(2), 2);
    EXPECT_EQ(PacBio::Numeric::NextPowerOfTwo(3), 4);
    EXPECT_EQ(PacBio::Numeric::NextPowerOfTwo(4), 4);
    EXPECT_EQ(PacBio::Numeric::NextPowerOfTwo(5), 8);
    EXPECT_EQ(PacBio::Numeric::NextPowerOfTwo(25), 32);
    EXPECT_EQ(PacBio::Numeric::NextPowerOfTwo(32), 32);
    EXPECT_EQ(PacBio::Numeric::NextPowerOfTwo(600), 1024);
}

TEST(Numeric, RoundUpDivision_poweroftwo_test)
{
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<1>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<1>(1), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<1>(2), 2);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<1>(3), 3);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<1>(4), 4);

    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(1), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(2), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(3), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(4), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(5), 2);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(7), 2);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<4>(8), 2);
}

TEST(Numeric, RoundUpDivision_nonpoweroftwo_test)
{
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(1), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(2), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(5), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(6), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(7), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(8), 2);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(9), 2);
    EXPECT_EQ(PacBio::Numeric::RoundUpDivision<7>(14), 2);
}

TEST(Numeric, RoundDownDivision_poweroftwo_test)
{
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<1>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<1>(1), 1);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<1>(2), 2);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<1>(3), 3);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<1>(4), 4);

    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(1), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(2), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(3), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(4), 1);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(5), 1);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(7), 1);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<4>(8), 2);
}

TEST(Numeric, RoundDownDivision_nonpoweroftwo_test)
{
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(1), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(2), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(5), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(6), 0);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(7), 1);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(8), 1);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(9), 1);
    EXPECT_EQ(PacBio::Numeric::RoundDownDivision<7>(14), 2);
}

TEST(Numeric, RoundUpToNextMultiple_poweroftwo_test)
{
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<1>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<1>(1), 1);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<1>(2), 2);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<1>(3), 3);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<1>(4), 4);

    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(1), 4);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(2), 4);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(3), 4);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(4), 4);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(5), 8);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(7), 8);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<4>(8), 8);
}

TEST(Numeric, RoundUpToNextMultiple_nonpoweroftwo_test)
{
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(0), 0);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(1), 7);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(2), 7);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(5), 7);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(6), 7);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(7), 7);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(8), 14);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(9), 14);
    EXPECT_EQ(PacBio::Numeric::RoundUpToNextMultiple<7>(14), 14);
}
