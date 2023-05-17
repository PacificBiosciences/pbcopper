#include <pbcopper/container/DNAString.h>

#include <gtest/gtest.h>

#include <string>

using namespace PacBio;

TEST(Container_DNAString, ConstexprFixed32)
{
    constexpr Container::DNA2bitStringFixed32 CONSTEXPR_BCT{"TCAGAGTCGTCCAAGG"};

    EXPECT_EQ(sizeof(CONSTEXPR_BCT), sizeof(std::uint32_t));
    EXPECT_EQ(CONSTEXPR_BCT.Capacity(), 16);

    EXPECT_EQ(CONSTEXPR_BCT[0], 3);
    EXPECT_EQ(CONSTEXPR_BCT[1], 1);
    EXPECT_EQ(CONSTEXPR_BCT[2], 0);
    EXPECT_EQ(CONSTEXPR_BCT[3], 2);
    EXPECT_EQ(CONSTEXPR_BCT[4], 0);
    EXPECT_EQ(CONSTEXPR_BCT[5], 2);
    EXPECT_EQ(CONSTEXPR_BCT[6], 3);
    EXPECT_EQ(CONSTEXPR_BCT[7], 1);
    EXPECT_EQ(CONSTEXPR_BCT[8], 2);
    EXPECT_EQ(CONSTEXPR_BCT[9], 3);
    EXPECT_EQ(CONSTEXPR_BCT[10], 1);
    EXPECT_EQ(CONSTEXPR_BCT[11], 1);
    EXPECT_EQ(CONSTEXPR_BCT[12], 0);
    EXPECT_EQ(CONSTEXPR_BCT[13], 0);
    EXPECT_EQ(CONSTEXPR_BCT[14], 2);
    EXPECT_EQ(CONSTEXPR_BCT[15], 2);

    auto revCt{CONSTEXPR_BCT};
    revCt.ReverseComplement();

    EXPECT_EQ(std::string{revCt}, "CCTTGGACGACTCTGA");
    EXPECT_EQ(revCt.RawData(), 0b00'10'11'01'11'01'00'10'01'00'10'10'11'11'01'01U);
}

TEST(Container_DNAString, ConstexprVariable32)
{
    constexpr Container::DNA2bitStringVariable32 CONSTEXPR_BCT{"TCAGAGTCGTCCAAGG"};

    EXPECT_EQ(sizeof(CONSTEXPR_BCT), 2 * sizeof(std::uint32_t));
    EXPECT_EQ(CONSTEXPR_BCT.Capacity(), 16);
    EXPECT_EQ(CONSTEXPR_BCT.Size(), 16);
    EXPECT_EQ(CONSTEXPR_BCT.Size(), CONSTEXPR_BCT.Length());

    EXPECT_EQ(CONSTEXPR_BCT[0], 3);
    EXPECT_EQ(CONSTEXPR_BCT[1], 1);
    EXPECT_EQ(CONSTEXPR_BCT[2], 0);
    EXPECT_EQ(CONSTEXPR_BCT[3], 2);
    EXPECT_EQ(CONSTEXPR_BCT[4], 0);
    EXPECT_EQ(CONSTEXPR_BCT[5], 2);
    EXPECT_EQ(CONSTEXPR_BCT[6], 3);
    EXPECT_EQ(CONSTEXPR_BCT[7], 1);
    EXPECT_EQ(CONSTEXPR_BCT[8], 2);
    EXPECT_EQ(CONSTEXPR_BCT[9], 3);
    EXPECT_EQ(CONSTEXPR_BCT[10], 1);
    EXPECT_EQ(CONSTEXPR_BCT[11], 1);
    EXPECT_EQ(CONSTEXPR_BCT[12], 0);
    EXPECT_EQ(CONSTEXPR_BCT[13], 0);
    EXPECT_EQ(CONSTEXPR_BCT[14], 2);
    EXPECT_EQ(CONSTEXPR_BCT[15], 2);
}

TEST(Container_DNAString, ConstexprVariable64ReverseCompl)
{
    constexpr Container::DNA2bitStringVariable64 CONSTEXPR_BCT{"TCAG"};

    EXPECT_EQ(sizeof(CONSTEXPR_BCT), 2 * sizeof(std::uint64_t));
    EXPECT_EQ(CONSTEXPR_BCT.Capacity(), 32);
    EXPECT_EQ(CONSTEXPR_BCT.Size(), 4);
    EXPECT_EQ(CONSTEXPR_BCT.Size(), CONSTEXPR_BCT.Length());

    EXPECT_EQ(CONSTEXPR_BCT[0], 3);
    EXPECT_EQ(CONSTEXPR_BCT[1], 1);
    EXPECT_EQ(CONSTEXPR_BCT[2], 0);
    EXPECT_EQ(CONSTEXPR_BCT[3], 2);

    auto revCt{CONSTEXPR_BCT};
    revCt.ReverseComplement();

    EXPECT_EQ(revCt[0], 1);
    EXPECT_EQ(revCt[1], 3);
    EXPECT_EQ(revCt[2], 2);
    EXPECT_EQ(revCt[3], 0);

    EXPECT_EQ(std::string{revCt}, "CTGA");
    EXPECT_EQ(revCt.RawData(), 0b00'10'11'01);
    EXPECT_EQ(revCt.Size(), 4);
}
