#include <gtest/gtest.h>

#include <pbcopper/pbmer/KFGraph.h>

TEST(Pbmer_KFGraph, each_node_has_read_id)
{
    const PacBio::Pbmer::Parser parser{7};
    const std::string td1{"CATACCAGCTTCCACAGACGGACGACAGATTGCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{7, 1};
    g.AddSeq(m1, 1, "A");
    EXPECT_TRUE(g.ValidateLoad());
    EXPECT_EQ(1, 1);
}

TEST(Pbmer_KFGraph, node_count_matches_kmer_count)
{
    const PacBio::Pbmer::Parser parser{7};
    const std::string td1{"CATACCAGCTTCCACAGACGGACGACAGATTGCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{7, 1};
    g.AddSeq(m1, 1, "A");
    EXPECT_TRUE(g.ValidateLoad());
    EXPECT_EQ(g.NNodes(), m1.size());
}

TEST(Pbmer_KFGraph, okay_edge_count)
{
    const PacBio::Pbmer::Parser parser{7};
    const std::string td1{"CATACCAGCTTCCACAGACGGACGACAGATTGCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{7, 1};
    g.AddSeq(m1, 1, "A");
    EXPECT_TRUE(g.ValidateLoad());
    EXPECT_EQ(g.OutEdgeCount(), m1.size() - 1);
}

TEST(Pbmer_KFGraph, node_count_matches_kmer_count_two_reads)
{
    const PacBio::Pbmer::Parser parser{7};
    const std::string td1{"CATACCAGCTTCCACAGACGGACGACAGATTGCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{7, 2};
    g.AddSeq(m1, 1, "A");
    g.AddSeq(m1, 2, "B");
    EXPECT_TRUE(g.ValidateLoad());
    EXPECT_EQ(g.NNodes(), m1.size());
}

TEST(Pbmer_KFGraph, no_collapse)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATcatCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{3, 1};
    g.AddSeq(m1, 1, "A");
    EXPECT_TRUE(g.ValidateLoad());
    EXPECT_EQ(g.NNodes(), m1.size());
    EXPECT_EQ(g.OutEdgeCount(), m1.size() - 1);
}

TEST(Pbmer_KFGraph, two_reads_no_collapse)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATcatCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{3, 2};
    g.AddSeq(m1, 1, "A");
    g.AddSeq(m1, 2, "B");
    EXPECT_TRUE(g.ValidateLoad());
    EXPECT_EQ(g.NNodes(), m1.size());
    EXPECT_EQ(g.OutEdgeCount(), m1.size() - 1);
}

TEST(Pbmer_KFGraph, two_reads_no_filter)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATcatCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{3, 2};
    g.AddSeq(m1, 1, "A");
    g.AddSeq(m1, 2, "B");
    g.SeqCountFilter(1, false);
    EXPECT_EQ(g.NNodes(), m1.size());
}

TEST(Pbmer_KFGraph, two_reads_all_nodes_filtered)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATcatCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{3, 2};
    g.AddSeq(m1, 1, "A");
    g.AddSeq(m1, 2, "B");
    g.SeqCountFilter(3, false);
    EXPECT_EQ(g.NNodes(), 0);
}

TEST(Pbmer_KFGraph, two_reads_one_node_filtered)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATcatCAT"};
    const std::string td2{"CATcatCATgat"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    std::vector<PacBio::Pbmer::DnaBit> m2 = parser.ParseDnaBit(td2);
    PacBio::Pbmer::KFG g{3, 2};
    g.AddSeq(m1, 1, "A");
    g.AddSeq(m2, 2, "B");
    EXPECT_EQ(g.NNodes(), 10);
    g.SeqCountFilter(2, false);
    EXPECT_EQ(g.NNodes(), 7);
}

TEST(Pbmer_KFGraph, two_reads_gt_filter_nothing_removed)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATcatCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    PacBio::Pbmer::KFG g{3, 2};
    g.AddSeq(m1, 1, "A");
    g.AddSeq(m1, 2, "B");
    g.SeqCountFilter(2, true);
    EXPECT_EQ(g.NNodes(), m1.size());
}

TEST(Pbmer_KFGraph, find_bubble)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    std::vector<PacBio::Pbmer::DnaBit> m2 = parser.ParseDnaBit(td2);

    PacBio::Pbmer::KFG g{7, 2};

    g.AddSeq(m1, 1, "A");
    g.AddSeq(m2, 2, "B");

    auto bubbles = g.FindBubbles();
    EXPECT_EQ(bubbles.size(), 1);
}

/*
This topology is not possible to find with a de bruijn graph.

    T : TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC(1861 - 1910) ||
        |||||||||||||||            ||||||||||||||||||||||| Q
      : TTCCAGTGCGGGAGG------------CACTGCTAGCATAGCCGGTAATC(1949 - 1986)
*/

TEST(Dbg_Bubbles, indel_found)
{
    const PacBio::Pbmer::Parser parser{15};
    //del                                (X)
    const std::string td1{"TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC"};
    const std::string td2{"TTCCAGTGCGGGAGGCACTGCTAGCATAGCCGGTAATC"};

    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    std::vector<PacBio::Pbmer::DnaBit> m2 = parser.ParseDnaBit(td2);

    PacBio::Pbmer::KFG g{15, 2};

    g.AddSeq(m1, 1, "A");
    g.AddSeq(m2, 2, "B");

    auto bubbles = g.FindBubbles();
    EXPECT_EQ(bubbles.size(), 1);
}
