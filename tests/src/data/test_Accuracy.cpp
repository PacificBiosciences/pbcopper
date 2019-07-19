// Author: Derek Barnett

#include <gtest/gtest.h>

#include <pbcopper/data/Accuracy.h>

using namespace PacBio;

TEST(Data_Accuracy, ClampValues)
{
    Data::Accuracy a_zero{0.0};
    Data::Accuracy a_neg{-0.5};
    Data::Accuracy a_min{0.0};
    Data::Accuracy a_normal{0.9};
    Data::Accuracy a_max{1.0};
    Data::Accuracy a_tooLarge{1.1};

    EXPECT_FLOAT_EQ(0.0, a_zero);
    EXPECT_FLOAT_EQ(0.0, a_neg);
    EXPECT_FLOAT_EQ(0.0, a_min);
    EXPECT_FLOAT_EQ(0.9, a_normal);
    EXPECT_FLOAT_EQ(1.0, a_max);
    EXPECT_FLOAT_EQ(1.0, a_tooLarge);
}
