#include <pbcopper/data/Accuracy.h>

#include <gtest/gtest.h>

using namespace PacBio;

TEST(Data_Accuracy, ClampValues)
{
    const Data::Accuracy a_zero{0.0};
    EXPECT_FLOAT_EQ(0.0, a_zero);

    const Data::Accuracy a_neg{-0.5};
    EXPECT_FLOAT_EQ(0.0, a_neg);

    const Data::Accuracy a_min{0.0};
    EXPECT_FLOAT_EQ(0.0, a_min);

    const Data::Accuracy a_normal{0.9};
    EXPECT_FLOAT_EQ(0.9, a_normal);

    const Data::Accuracy a_max{1.0};
    EXPECT_FLOAT_EQ(1.0, a_max);

    const Data::Accuracy a_tooLarge{1.1};
    EXPECT_FLOAT_EQ(1.0, a_tooLarge);

    const Data::Accuracy a_minus_one_missed{-1.0001};
    EXPECT_FLOAT_EQ(0.0, a_minus_one_missed);

    const Data::Accuracy a_minus_one{-1.0};
    EXPECT_FLOAT_EQ(-1.0, a_minus_one);

    const Data::Accuracy a_tooSmall{-1.1};
    EXPECT_FLOAT_EQ(0.0, a_tooSmall);
}
