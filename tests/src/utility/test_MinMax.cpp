
#include <gtest/gtest.h>

#include <pbcopper/utility/MinMax.h>

TEST(Utility_MinMax, can_find_maximum_integer_from_two_args)
{
    EXPECT_EQ(-2, PacBio::Utility::Max(-4, -2));
    EXPECT_EQ(0, PacBio::Utility::Max(-2, 0));
    EXPECT_EQ(8, PacBio::Utility::Max(8, 0));
    EXPECT_EQ(5, PacBio::Utility::Max(4, 5));
    EXPECT_EQ(4, PacBio::Utility::Max(4, 4));
}

TEST(Utility_MinMax, can_find_maximum_integer_from_multiple_args)
{
    EXPECT_EQ(-2, PacBio::Utility::Max(-4, -2, -6, -3));
    EXPECT_EQ(0, PacBio::Utility::Max(-2, 0, -6, -8));
    EXPECT_EQ(8, PacBio::Utility::Max(8, 0, 2, -3));
    EXPECT_EQ(5, PacBio::Utility::Max(4, 5, 3, -3));
    EXPECT_EQ(4, PacBio::Utility::Max(4, 4, 4, 4));
}

TEST(Utility_MinMax, can_find_maximum_double_from_two_args)
{
    EXPECT_DOUBLE_EQ(-2.0, PacBio::Utility::Max(-4.0, -2.0));
    EXPECT_DOUBLE_EQ(0.0, PacBio::Utility::Max(-2.0, 0.0));
    EXPECT_DOUBLE_EQ(8.0, PacBio::Utility::Max(8.0, 0.0));
    EXPECT_DOUBLE_EQ(5.0, PacBio::Utility::Max(4.0, 5.0));
    EXPECT_DOUBLE_EQ(4.0, PacBio::Utility::Max(4.0, 4.0));
}

TEST(Utility_MinMax, can_find_maximum_double_from_multiple_args)
{
    EXPECT_DOUBLE_EQ(-2.0, PacBio::Utility::Max(-4.0, -2.0, -6.0, -3.0));
    EXPECT_DOUBLE_EQ(0.0, PacBio::Utility::Max(-2.0, 0.0, -6.0, -8.0));
    EXPECT_DOUBLE_EQ(8.0, PacBio::Utility::Max(8.0, 0.0, 2.0, -3.0));
    EXPECT_DOUBLE_EQ(5.0, PacBio::Utility::Max(4.0, 5.0, 3.0, -3.0));
    EXPECT_DOUBLE_EQ(4.0, PacBio::Utility::Max(4.0, 4.0, 4.0, 4.0));
}

TEST(Utility_MinMax, can_find_minimum_integer_from_two_args)
{
    EXPECT_EQ(-4, PacBio::Utility::Min(-4, -2));
    EXPECT_EQ(-2, PacBio::Utility::Min(-2, 0));
    EXPECT_EQ(0, PacBio::Utility::Min(8, 0));
    EXPECT_EQ(4, PacBio::Utility::Min(4, 5));
    EXPECT_EQ(4, PacBio::Utility::Min(4, 4));
}

TEST(Utility_MinMax, can_find_minimum_integer_from_multiple_args)
{
    EXPECT_EQ(-4, PacBio::Utility::Min(-4, -2, -3, -1));
    EXPECT_EQ(-2, PacBio::Utility::Min(-2, 0, 8, 4));
    EXPECT_EQ(0, PacBio::Utility::Min(8, 0, 5, 3));
    EXPECT_EQ(4, PacBio::Utility::Min(4, 5, 5, 9));
    EXPECT_EQ(4, PacBio::Utility::Min(4, 4, 4, 4));
}

TEST(Utility_MinMax, can_find_minimum_double_from_two_args)
{
    EXPECT_DOUBLE_EQ(-4.0, PacBio::Utility::Min(-4.0, -2.0));
    EXPECT_DOUBLE_EQ(-2.0, PacBio::Utility::Min(-2.0, 0.0));
    EXPECT_DOUBLE_EQ(0.0, PacBio::Utility::Min(8.0, 0.0));
    EXPECT_DOUBLE_EQ(4.0, PacBio::Utility::Min(4.0, 5.0));
    EXPECT_DOUBLE_EQ(4.0, PacBio::Utility::Min(4.0, 4.0));
}

TEST(Utility_MinMax, can_find_minimum_double_from_multiple_args)
{
    EXPECT_DOUBLE_EQ(-4.0, PacBio::Utility::Min(-4.0, -2.0, -3.0, -1.0));
    EXPECT_DOUBLE_EQ(-2.0, PacBio::Utility::Min(-2.0, 0.0, 8.0, 4.0));
    EXPECT_DOUBLE_EQ(0.0, PacBio::Utility::Min(8.0, 0.0, 5.0, 3.0));
    EXPECT_DOUBLE_EQ(4.0, PacBio::Utility::Min(4.0, 5.0, 5.0, 9.0));
    EXPECT_DOUBLE_EQ(4.0, PacBio::Utility::Min(4.0, 4.0, 4.0, 4.0));
}