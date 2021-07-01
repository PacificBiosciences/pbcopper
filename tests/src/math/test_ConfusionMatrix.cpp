#include <pbcopper/math/ConfusionMatrix.h>

#include <gtest/gtest.h>

using namespace PacBio;

TEST(Math_ConfusionMatrix, can_compute_basic_metrics)
{
    // n:40, n_pos:20, n_neg:20

    constexpr const double err = 0.00001;

    Math::ConfusionMatrix c;
    c.NumTP = 18;
    c.NumTN = 16;
    c.NumFP = 4;
    c.NumFN = 2;
    EXPECT_EQ(40, c.Size());

    EXPECT_NEAR(0.9, c.Recall(), err);
    EXPECT_NEAR(0.9, c.Sensitivity(), err);
    EXPECT_NEAR(0.9, c.TruePositiveRate(), err);
    EXPECT_NEAR(0.8, c.Specificity(), err);
    EXPECT_NEAR(0.8, c.TrueNegativeRate(), err);
    EXPECT_NEAR(0.81818, c.Precision(), err);
    EXPECT_NEAR(0.81818, c.PPV(), err);
    EXPECT_NEAR(0.88888, c.NPV(), err);
    EXPECT_NEAR(0.85, c.Accuracy(), err);
    EXPECT_NEAR(0.85714, c.F1Score(), err);
    EXPECT_NEAR(0.7, c.Informedness(), err);
    EXPECT_NEAR(0.18181, c.FalseDiscoveryRate(), err);
    EXPECT_NEAR(0.11111, c.FalseOmissionRate(), err);
    EXPECT_NEAR(0.2, c.FalsePositiveRate(), err);
    EXPECT_NEAR(0.1, c.FalseNegativeRate(), err);
}
