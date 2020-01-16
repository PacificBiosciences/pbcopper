#include <gtest/gtest.h>

#include <pbcopper/pbmer/DnaBit.h>

TEST(DnaBit_cmp, check_eq_positive)
{
    const PacBio::Pbmer::DnaBit k1{2862426843, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 0, 16};
    EXPECT_TRUE(k1 == k2);
}

TEST(DnaBit_cmp, check_eq_negative)
{
    const PacBio::Pbmer::DnaBit k1{2862426843, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 1, 16};
    const PacBio::Pbmer::DnaBit k3{2862426841, 0, 16};
    EXPECT_FALSE(k1 == k2);
    EXPECT_FALSE(k1 == k3);
}

TEST(DnaBit_cmp, check_lt)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 0, 16};
    EXPECT_TRUE(k1 < k2);
}

TEST(DnaBit_cmp, check_gt)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 0, 16};
    EXPECT_TRUE(k2 > k1);
}

TEST(DnaBit_cmp, check_lt_eq)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426841, 0, 16};
    EXPECT_TRUE(k1 <= k2);
}

TEST(DnaBit_cmp, check_gt_eq)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426841, 0, 16};
    EXPECT_TRUE(k1 >= k2);
}

TEST(DnaBit_str, check_string)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ("GGGGGCTCAGGGTCGC", k1.KmerToStr());
}

TEST(DnaBit_rc, check_rc)
{
    PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    k1.ReverseComp();
    EXPECT_EQ("GCGACCCTGAGCCCCC", k1.KmerToStr());
    EXPECT_EQ(1, k1.strand);
}

TEST(DnaBit_app, check_correct_append)
{
    PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ("GGGGGCTCAGGGTCGC", k1.KmerToStr());
    k1.AppendBase('T');
    EXPECT_EQ("GGGGCTCAGGGTCGCT", k1.KmerToStr());
    k1.AppendBase('A');
    EXPECT_EQ("GGGCTCAGGGTCGCTA", k1.KmerToStr());
    k1.AppendBase('G');
    EXPECT_EQ("GGCTCAGGGTCGCTAG", k1.KmerToStr());
    k1.AppendBase('C');
    EXPECT_EQ("GCTCAGGGTCGCTAGC", k1.KmerToStr());
    k1.AppendBase('a');
    EXPECT_EQ("CTCAGGGTCGCTAGCA", k1.KmerToStr());
    k1.AppendBase('t');
    EXPECT_EQ("TCAGGGTCGCTAGCAT", k1.KmerToStr());
    k1.AppendBase('g');
    EXPECT_EQ("CAGGGTCGCTAGCATG", k1.KmerToStr());
    k1.AppendBase('c');
    EXPECT_EQ("AGGGTCGCTAGCATGC", k1.KmerToStr());
}

TEST(DnaBit_preapp, check_correct_preappend)
{
    PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ("GGGGGCTCAGGGTCGC", k1.KmerToStr());
    k1.PrependBase('T');
    EXPECT_EQ("TGGGGGCTCAGGGTCG", k1.KmerToStr());
    k1.PrependBase('A');
    EXPECT_EQ("ATGGGGGCTCAGGGTC", k1.KmerToStr());
    k1.PrependBase('G');
    EXPECT_EQ("GATGGGGGCTCAGGGT", k1.KmerToStr());
}
