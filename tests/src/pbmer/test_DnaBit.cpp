#include <array>

#include <gtest/gtest.h>

#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/pbmer/Parser.h>

TEST(Pbmer_DnaBit, cmp_check_eq_positive)
{
    const PacBio::Pbmer::DnaBit k1{2862426843, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 0, 16};
    EXPECT_TRUE(k1 == k2);
}

TEST(Pbmer_DnaBit, cmp_check_eq_negative)
{
    const PacBio::Pbmer::DnaBit k1{2862426843, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 1, 16};
    const PacBio::Pbmer::DnaBit k3{2862426841, 0, 16};
    EXPECT_FALSE(k1 == k2);
    EXPECT_FALSE(k1 == k3);
}

TEST(Pbmer_DnaBit, cmp_check_lt)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 0, 16};
    EXPECT_TRUE(k1 < k2);
}

TEST(Pbmer_DnaBit, cmp_check_gt)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426843, 0, 16};
    EXPECT_TRUE(k2 > k1);
}

TEST(Pbmer_DnaBit, cmp_check_lt_eq)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426841, 0, 16};
    EXPECT_TRUE(k1 <= k2);
}

TEST(Pbmer_DnaBit, cmp_check_gt_eq)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    const PacBio::Pbmer::DnaBit k2{2862426841, 0, 16};
    EXPECT_TRUE(k1 >= k2);
}

TEST(Pbmer_DnaBit, str_check_string)
{
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ("GGGGGCTCAGGGTCGC", k1.KmerToStr());
}

TEST(Pbmer_DnaBit, str_first_base_idx_a)
{
    const std::array<char, 8> bases{'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
    //"GGGGGCTCAGGGTCGC"
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ(2, k1.GetFirstBaseIdx());
    EXPECT_EQ('G', bases[k1.GetFirstBaseIdx()]);
}

TEST(Pbmer_DnaBit, str_first_base_idx_b)
{
    const std::array<char, 8> bases{'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
    //"AGGGGGCTCAGGGTCG"
    const PacBio::Pbmer::DnaBit k1{2862426841 >> 2, 0, 16};
    EXPECT_EQ(0, k1.GetFirstBaseIdx());
    EXPECT_EQ('A', bases[k1.GetFirstBaseIdx()]);
}

TEST(Pbmer_DnaBit, str_last_base_idx_a)
{
    //"GGGGGCTCAGGGTCGC"
    const std::array<char, 8> bases{'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ(1, k1.GetLastBaseIdx());
    EXPECT_EQ('C', bases[k1.GetLastBaseIdx()]);
}

TEST(Pbmer_DnaBit, str_last_base_idx_b)
{
    const std::array<char, 8> bases{'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
    //"GGGGGCTCAGGGTCGC"
    const PacBio::Pbmer::DnaBit k1{2862426840, 0, 16};
    EXPECT_EQ(0, k1.GetLastBaseIdx());
    EXPECT_EQ('A', bases[k1.GetLastBaseIdx()]);
}

TEST(Pbmer_DnaBit, str_first_base_rc_idx_a)
{
    const std::array<char, 8> bases{'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
    //"GGGGGCTCAGGGTCGC"
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ(2, k1.GetFirstBaseRCIdx());
    EXPECT_EQ('G', bases[k1.GetFirstBaseRCIdx()]);
}

TEST(Pbmer_DnaBit, str_last_base_rc_idx_a)
{
    const std::array<char, 8> bases{'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
    //"GGGGGCTCAGGGTCGC"
    const PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    EXPECT_EQ(1, k1.GetLastBaseRCIdx());
    EXPECT_EQ('C', bases[k1.GetLastBaseRCIdx()]);
}

TEST(Pbmer_DnaBit, rc_check_rc)
{
    PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    k1.ReverseComp();
    EXPECT_EQ("GCGACCCTGAGCCCCC", k1.KmerToStr());
    EXPECT_EQ(1, k1.strand);
}

TEST(Pbmer_DnaBit, app_check_correct_append)
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

TEST(Pbmer_DnaBit, preapp_check_correct_preappend)
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

TEST(Pbmer_DnaBit, bin_pack_unpack)
{
    PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    auto seen = k1.DnaBit2Bin();
    EXPECT_GT(seen, 0);
    PacBio::Pbmer::DnaBit k2;
    k2.Bin2DnaBit(seen);
    EXPECT_EQ(k1, k2);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_homopolymer_long)
{
    PacBio::Pbmer::DnaBit k1{20753, 0, 16};
    //1.2.3.4.
    //AAAAAAAACCACACAC
    std::cerr << k1.KmerToStr() << "\n";
    EXPECT_EQ(k1.LongestDiNucRun(), 4);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_homopolymer_short)
{
    PacBio::Pbmer::DnaBit k1{2862426841, 0, 16};
    k1.ReverseComp();
    //             1.2
    // "GCGACCCTGAGCCCCC"
    EXPECT_EQ(k1.LongestDiNucRun(), 2);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_ta_run)
{
    PacBio::Pbmer::DnaBit k1{3355443, 0, 16};
    //       6.5.4.3.2.1
    // "AAAAATATATATATAT"
    EXPECT_EQ(k1.LongestDiNucRun(), 6);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_three_mer)
{
    PacBio::Pbmer::DnaBit k1{0, 0, 3};
    //  1.1
    // "AAA"
    EXPECT_EQ(k1.LongestDiNucRun(), 1);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_two_mers)
{
    PacBio::Pbmer::DnaBit k1{0, 0, 2};
    //  1.
    // "AA"
    EXPECT_EQ(k1.LongestDiNucRun(), 1);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_two_mer_otherbase)
{
    PacBio::Pbmer::DnaBit k1{3, 0, 2};
    EXPECT_EQ(k1.LongestDiNucRun(), 1);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_one)
{
    PacBio::Pbmer::DnaBit k1{3, 0, 1};
    EXPECT_EQ(k1.LongestDiNucRun(), 0);
}

TEST(Pbmer_DnaBit, correct_dinuc_count_zero)
{
    PacBio::Pbmer::DnaBit k1{0, 0, 0};
    EXPECT_EQ(k1.LongestDiNucRun(), 0);
}

TEST(Pbmer_DnaBit, correct_dinuc_long_kmer)
{
    const std::string kmerstr = "TCAGAGCGCTGTGAGACAGCACTCTGA";
    const PacBio::Pbmer::Parser parser{static_cast<uint8_t>(27)};
    std::vector<PacBio::Pbmer::DnaBit> one = parser.ParseDnaBit(kmerstr);
    EXPECT_EQ(int(one.front().LongestDiNucRun()), 2);
}
