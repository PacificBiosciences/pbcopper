#include <pbcopper/math/ConfusionMatrix.h>

#include <gtest/gtest.h>

using namespace PacBio;

TEST(Math_ConfusionMatrix, can_compute_basic_metrics)
{
    // n:40, n_pos:20, n_neg:20

    constexpr const double ERR = 0.00001;

    Math::ConfusionMatrix c;
    c.NumTP = 18;
    c.NumTN = 16;
    c.NumFP = 4;
    c.NumFN = 2;
    EXPECT_EQ(40, c.Size());

    EXPECT_NEAR(0.9, c.Recall(), ERR);
    EXPECT_NEAR(0.9, c.Sensitivity(), ERR);
    EXPECT_NEAR(0.9, c.TruePositiveRate(), ERR);
    EXPECT_NEAR(0.8, c.Specificity(), ERR);
    EXPECT_NEAR(0.8, c.TrueNegativeRate(), ERR);
    EXPECT_NEAR(0.81818, c.Precision(), ERR);
    EXPECT_NEAR(0.81818, c.PPV(), ERR);
    EXPECT_NEAR(0.88888, c.NPV(), ERR);
    EXPECT_NEAR(0.85, c.Accuracy(), ERR);
    EXPECT_NEAR(0.85714, c.F1Score(), ERR);
    EXPECT_NEAR(0.7, c.Informedness(), ERR);
    EXPECT_NEAR(0.18181, c.FalseDiscoveryRate(), ERR);
    EXPECT_NEAR(0.11111, c.FalseOmissionRate(), ERR);
    EXPECT_NEAR(0.2, c.FalsePositiveRate(), ERR);
    EXPECT_NEAR(0.1, c.FalseNegativeRate(), ERR);
}

TEST(Math_ConfusionMatrix, can_merge)
{
    constexpr const double ERR = 0.00001;

    Math::ConfusionMatrix c1{12, 5, 1, 0};
    Math::ConfusionMatrix c2{6, 11, 3, 2};

    c1 += c2;

    EXPECT_NEAR(0.9, c1.Recall(), ERR);
    EXPECT_NEAR(0.9, c1.Sensitivity(), ERR);
    EXPECT_NEAR(0.9, c1.TruePositiveRate(), ERR);
    EXPECT_NEAR(0.8, c1.Specificity(), ERR);
    EXPECT_NEAR(0.8, c1.TrueNegativeRate(), ERR);
    EXPECT_NEAR(0.81818, c1.Precision(), ERR);
    EXPECT_NEAR(0.81818, c1.PPV(), ERR);
    EXPECT_NEAR(0.88888, c1.NPV(), ERR);
    EXPECT_NEAR(0.85, c1.Accuracy(), ERR);
    EXPECT_NEAR(0.85714, c1.F1Score(), ERR);
    EXPECT_NEAR(0.7, c1.Informedness(), ERR);
    EXPECT_NEAR(0.18181, c1.FalseDiscoveryRate(), ERR);
    EXPECT_NEAR(0.11111, c1.FalseOmissionRate(), ERR);
    EXPECT_NEAR(0.2, c1.FalsePositiveRate(), ERR);
    EXPECT_NEAR(0.1, c1.FalseNegativeRate(), ERR);
}
