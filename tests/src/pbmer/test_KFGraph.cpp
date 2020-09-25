#include <pbcopper/pbmer/KFGraph.h>

#include <gtest/gtest.h>

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

TEST(Pbmer_KFGraph, indel_found)
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

TEST(Pbmer_KFGraph, expected_GFA)
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

    std::string expected = R"(H	VN:Z:1.0
S	11205503064534538081	ATAGCCGGTAATC	KC:i:13	LN:i:13	RC:i:26
S	16273463927145580620	CACTGCTAGC	KC:i:10	LN:i:10	RC:i:11
S	4268400356317211862	AGGAGGAGGAGGCACTGCTAGC	KC:i:22	LN:i:22	RC:i:23
S	9657151653063332195	TTCCAGTGCGGGAGG	KC:i:1	LN:i:15	RC:i:2
L	16273463927145580620	+	11205503064534538081	+	*
L	4268400356317211862	+	11205503064534538081	+	*
L	9657151653063332195	+	16273463927145580620	+	*
L	9657151653063332195	+	4268400356317211862	+	*
)";

    auto seen = g.DumpGFAUtgs();

    EXPECT_EQ(seen, expected);
}

TEST(Pbmer_KFGraph, expected_GFA_linear)
{
    const PacBio::Pbmer::Parser parser{15};

    const std::string td1{"TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC"};
    const std::string td2{"TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC"};

    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    std::vector<PacBio::Pbmer::DnaBit> m2 = parser.ParseDnaBit(td2);

    PacBio::Pbmer::KFG g{15, 2};

    g.AddSeq(m1, 1, "A");
    g.AddSeq(m2, 2, "B");

    auto seen = g.DumpGFAUtgs();

    std::string expected = R"(H	VN:Z:1.0
S	9657151653063332195	TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC	KC:i:36	LN:i:50	RC:i:72
)";

    EXPECT_EQ(seen, expected);
}

TEST(Pbmer_KFGraph, tail_diff)
{
    const PacBio::Pbmer::Parser parser{15};

    //       TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAAT
    // one:  TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAA
    // two:                                                  T
    // three:                                                C

    const std::string td1{"TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATT"};
    const std::string td2{"TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC"};

    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);
    std::vector<PacBio::Pbmer::DnaBit> m2 = parser.ParseDnaBit(td2);

    PacBio::Pbmer::KFG g{15, 2};

    g.AddSeq(m1, 1, "A");
    g.AddSeq(m2, 2, "B");

    auto seen = g.DumpGFAUtgs();

    std::string expected = R"(H	VN:Z:1.0
S	12648310277698350581	C	KC:i:1	LN:i:1	RC:i:1
S	5024143364061459383	T	KC:i:1	LN:i:1	RC:i:1
S	9657151653063332195	TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAAT	KC:i:35	LN:i:49	RC:i:70
L	9657151653063332195	+	12648310277698350581	+	*
L	9657151653063332195	+	5024143364061459383	+	*
)";

    EXPECT_EQ(seen, expected);
}
