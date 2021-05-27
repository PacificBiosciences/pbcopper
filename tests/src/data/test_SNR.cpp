#include <pbcopper/data/SNR.h>

#include <gtest/gtest.h>

const PacBio::Data::SNR snr1{0.25, 3.5, 9.75, 20.25};
const PacBio::Data::SNR snr2{3.25, 0.5, 7.25, 19.0};

TEST(Data_SNR, Basic)
{
    const PacBio::Data::SNR snr{0.25, 3.5, 9.75, 20.25};

    EXPECT_EQ(snr[0], 0.25);
    EXPECT_EQ(snr[1], 3.5);
    EXPECT_EQ(snr[2], 9.75);
    EXPECT_EQ(snr[3], 20.25);

    EXPECT_EQ(snr.Minimum(), 0.25);

    const std::vector<float> snrVec{snr};
    EXPECT_EQ(snrVec[0], 0.25);
    EXPECT_EQ(snrVec[1], 3.5);
    EXPECT_EQ(snrVec[2], 9.75);
    EXPECT_EQ(snrVec[3], 20.25);

    EXPECT_EQ(snr, snr1);
    EXPECT_NE(snr, snr2);
}

TEST(Data_SNR, FromStdVector)
{
    const std::vector<float> arr{0.25, 3.5, 9.75, 20.25};
    const PacBio::Data::SNR snr{arr};

    EXPECT_EQ(snr[0], 0.25);
    EXPECT_EQ(snr[1], 3.5);
    EXPECT_EQ(snr[2], 9.75);
    EXPECT_EQ(snr[3], 20.25);

    EXPECT_EQ(snr.Minimum(), 0.25);

    const std::vector<float> snrVec{snr};
    EXPECT_EQ(snrVec[0], 0.25);
    EXPECT_EQ(snrVec[1], 3.5);
    EXPECT_EQ(snrVec[2], 9.75);
    EXPECT_EQ(snrVec[3], 20.25);

    EXPECT_EQ(snr, snr1);
    EXPECT_NE(snr, snr2);
}

TEST(Data_SNR, FromCArray)
{
    const float arr[] = {0.25, 3.5, 9.75, 20.25};
    const PacBio::Data::SNR snr{arr};

    EXPECT_EQ(snr[0], 0.25);
    EXPECT_EQ(snr[1], 3.5);
    EXPECT_EQ(snr[2], 9.75);
    EXPECT_EQ(snr[3], 20.25);

    EXPECT_EQ(snr.Minimum(), 0.25);

    const std::vector<float> snrVec{snr};
    EXPECT_EQ(snrVec[0], 0.25);
    EXPECT_EQ(snrVec[1], 3.5);
    EXPECT_EQ(snrVec[2], 9.75);
    EXPECT_EQ(snrVec[3], 20.25);

    EXPECT_EQ(snr, snr1);
    EXPECT_NE(snr, snr2);
}
