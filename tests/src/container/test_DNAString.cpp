#include <pbcopper/container/DNAString.h>

#include <gtest/gtest.h>

#include <string>

using namespace PacBio;

TEST(Container_DNAString, ConstexprFixed32)
{
    constexpr Container::DNA2bitStringFixed32 constexprBct{"TCAGAGTCGTCCAAGG"};

    EXPECT_EQ(sizeof(constexprBct), sizeof(uint32_t));
    EXPECT_EQ(constexprBct.Capacity(), 16);

    EXPECT_EQ(constexprBct[0], 3);
    EXPECT_EQ(constexprBct[1], 1);
    EXPECT_EQ(constexprBct[2], 0);
    EXPECT_EQ(constexprBct[3], 2);
    EXPECT_EQ(constexprBct[4], 0);
    EXPECT_EQ(constexprBct[5], 2);
    EXPECT_EQ(constexprBct[6], 3);
    EXPECT_EQ(constexprBct[7], 1);
    EXPECT_EQ(constexprBct[8], 2);
    EXPECT_EQ(constexprBct[9], 3);
    EXPECT_EQ(constexprBct[10], 1);
    EXPECT_EQ(constexprBct[11], 1);
    EXPECT_EQ(constexprBct[12], 0);
    EXPECT_EQ(constexprBct[13], 0);
    EXPECT_EQ(constexprBct[14], 2);
    EXPECT_EQ(constexprBct[15], 2);

    auto revCt{constexprBct};
    revCt.ReverseComp();

    EXPECT_EQ(std::string{revCt}, "CCTTGGACGACTCTGA");
    EXPECT_EQ(revCt.RawData(), 0b00'10'11'01'11'01'00'10'01'00'10'10'11'11'01'01U);
}

TEST(Container_DNAString, ConstexprVariable32)
{
    constexpr Container::DNA2bitStringVariable32 constexprBct{"TCAGAGTCGTCCAAGG"};

    EXPECT_EQ(sizeof(constexprBct), 2 * sizeof(uint32_t));
    EXPECT_EQ(constexprBct.Capacity(), 16);
    EXPECT_EQ(constexprBct.Size(), 16);

    EXPECT_EQ(constexprBct[0], 3);
    EXPECT_EQ(constexprBct[1], 1);
    EXPECT_EQ(constexprBct[2], 0);
    EXPECT_EQ(constexprBct[3], 2);
    EXPECT_EQ(constexprBct[4], 0);
    EXPECT_EQ(constexprBct[5], 2);
    EXPECT_EQ(constexprBct[6], 3);
    EXPECT_EQ(constexprBct[7], 1);
    EXPECT_EQ(constexprBct[8], 2);
    EXPECT_EQ(constexprBct[9], 3);
    EXPECT_EQ(constexprBct[10], 1);
    EXPECT_EQ(constexprBct[11], 1);
    EXPECT_EQ(constexprBct[12], 0);
    EXPECT_EQ(constexprBct[13], 0);
    EXPECT_EQ(constexprBct[14], 2);
    EXPECT_EQ(constexprBct[15], 2);
}

TEST(Container_DNAString, ConstexprVariable64ReverseCompl)
{
    constexpr Container::DNA2bitStringVariable64 constexprBct{"TCAG"};

    EXPECT_EQ(sizeof(constexprBct), 2 * sizeof(uint64_t));
    EXPECT_EQ(constexprBct.Capacity(), 32);
    EXPECT_EQ(constexprBct.Size(), 4);

    EXPECT_EQ(constexprBct[0], 3);
    EXPECT_EQ(constexprBct[1], 1);
    EXPECT_EQ(constexprBct[2], 0);
    EXPECT_EQ(constexprBct[3], 2);

    auto revCt{constexprBct};
    revCt.ReverseComp();

    EXPECT_EQ(revCt[0], 1);
    EXPECT_EQ(revCt[1], 3);
    EXPECT_EQ(revCt[2], 2);
    EXPECT_EQ(revCt[3], 0);

    EXPECT_EQ(std::string{revCt}, "CTGA");
    EXPECT_EQ(revCt.RawData(), 0b00'10'11'01);
    EXPECT_EQ(revCt.Size(), 4);
}
