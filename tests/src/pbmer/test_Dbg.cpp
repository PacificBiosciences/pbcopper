#include <gtest/gtest.h>

#include <pbcopper/pbmer/Dbg.h>

TEST(Pbmer_Dbg, add_kmers_throws_if_kmer_too_big)
{
    const PacBio::Pbmer::Parser parser{32};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{32, 1};

    EXPECT_EQ(dg.AddKmers(m, 1), -1);
}

TEST(Pbmer_Dbg, add_kmers_throws_on_wrong_width)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{16, 1};

    EXPECT_EQ(dg.AddKmers(m, 1), -2);
}

// TODO write a test to cover the multi add. Ivan pointed out the logic is probably wrong.
TEST(Pbmer_Dbg, AddKmersMany_check_throw_wrong_width) {}

TEST(Pbmer_Dbg, check_dump)
{
    const PacBio::Pbmer::Parser parser{5};
    const std::string td1{"CATACGTCCGTG"};
    const std::string td2{"CATACGTCCGTG"};
    const std::string td3{"CATACGTCCGTG"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};

    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Mers m3{parser.Parse(td3)};

    PacBio::Pbmer::Dbg dg{5, 3};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);

    dg.BuildEdges();
    // TODO this should not be a dummy.
    EXPECT_EQ(1, 1);
}

TEST(Pbmer_Dbg, test_topo_three_kmers)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATAG"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Dbg dg{3, 1};
    dg.AddKmers(m1, 1);

    EXPECT_EQ(dg.NNodes(), 3);

    dg.BuildEdges();

    std::string expected = R"(digraph DBGraph {
    ATG [fillcolor=red, style="rounded,filled", shape=diamond]
    CTA [fillcolor=red, style="rounded,filled", shape=diamond]
    ATA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    ATG -> ATA;
    CTA -> ATA;
    ATA -> CTA;
    ATA -> ATG;
})";

    std::string seen = dg.Graph2StringDot();
    EXPECT_EQ(dg.ValidateEdges(), true);
    EXPECT_EQ(seen, expected);
}

TEST(Pbmer_Dbg, test_topo_five_kmers)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATACCT"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Dbg dg{3, 1};
    dg.AddKmers(m1, 1);

    EXPECT_EQ(dg.NNodes(), 5);

    dg.BuildEdges();

    std::string expected = R"(digraph DBGraph {
    ACC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    AGG [fillcolor=red, style="rounded,filled", shape=diamond]
    GTA [fillcolor=red, style="rounded,filled", shape=diamond]
    ATG [fillcolor=red, style="rounded,filled", shape=diamond]
    ATA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    ACC -> AGG;
    ACC -> GTA;
    AGG -> ACC;
    GTA -> ATA;
    GTA -> ACC;
    ATG -> ATA;
    ATA -> GTA;
    ATA -> ATG;
})";

    std::string seen = dg.Graph2StringDot();
    EXPECT_EQ(seen, expected);
}

TEST(Pbmer_Dbg, test_topo_five_kmers_add_verifed_edges)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"CATACCT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);

    PacBio::Pbmer::Dbg dg{3, 1};

    dg.AddVerifedKmerPairs(m1, 1);

    EXPECT_EQ(dg.NNodes(), 5);

    std::string expected = R"(digraph DBGraph {
    ACC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    AGG [fillcolor=red, style="rounded,filled", shape=diamond]
    GTA [fillcolor=red, style="rounded,filled", shape=diamond]
    ATG [fillcolor=red, style="rounded,filled", shape=diamond]
    ATA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    ACC -> AGG;
    ACC -> GTA;
    AGG -> ACC;
    GTA -> ATA;
    GTA -> ACC;
    ATG -> ATA;
    ATA -> GTA;
    ATA -> ATG;
})";

    std::string seen = dg.Graph2StringDot();
    EXPECT_EQ(seen, expected);
}

TEST(Pbmer_Dbg, can_validate_verified_edges)
{
    const PacBio::Pbmer::Parser parser{7};
    const std::string td1{"CATACCAGCTTCCACAGACGGACGACAGATTGCAT"};
    std::vector<PacBio::Pbmer::DnaBit> m1 = parser.ParseDnaBit(td1);

    PacBio::Pbmer::Dbg dg{7, 1};

    dg.AddVerifedKmerPairs(m1, 1);

    EXPECT_EQ(dg.ValidateEdges(), true);
}

TEST(Pbmer_Dbg, can_validate_load)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"ATCGCT"};
    const std::string td2{"ATCGAT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{3, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    EXPECT_TRUE(dg.ValidateLoad());
}

TEST(Pbmer_Dbg, can_validate_edges)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    EXPECT_TRUE(dg.ValidateEdges());
}

TEST(Pbmer_Dbg, FrequencyFilterNodes_empty)
{

    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    dg.FrequencyFilterNodes(10);
    EXPECT_EQ(dg.NNodes(), 0);
}

TEST(Pbmer_Dbg, FrequencyFilterNodes2_empty)
{

    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    dg.FrequencyFilterNodes2(10);
    EXPECT_EQ(dg.NNodes(), 0);
}

TEST(Pbmer_Dbg, FrequencyFilterNodes2_gt1)
{

    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    dg.FrequencyFilterNodes2(0, true);
    EXPECT_EQ(dg.NNodes(), 0);
}

TEST(Pbmer_Dbg, FrequencyFilterNodes2_gt1_lt2)
{

    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    dg.FrequencyFilterNodes2(1, true);
    dg.FrequencyFilterNodes2(1, false);
    // 22 nodes 8 of which span the bubble path;
    EXPECT_EQ(dg.NNodes(), 14);
}

TEST(Pbmer_Dbg, FrequencyFilterNodes_one_left)
{

    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td3{"ATGGAAG"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};
    PacBio::Pbmer::Mers m3{parser.Parse(td3)};

    PacBio::Pbmer::Dbg dg{7, 3};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);

    dg.BuildEdges();
    dg.FrequencyFilterNodes(3);
    EXPECT_EQ(dg.NNodes(), 1);
}

TEST(Pbmer_Dbg, GetNBreadth_bubble_found)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 1);
}

TEST(Pbmer_Dbg, GetNBreadth_node_count_correct)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"GATTTGTTCCGCGACTTCCAT"};
    //ATGGAAGTGGCGGAACAAATC

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(dg.NNodes(), 15);
    EXPECT_EQ(bubbles.size(), 0);
}

TEST(Pbmer_Dbg, GetNBreadth_bubble_found_rc_first)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"GATTTGTTCCGCCACTTCCAT"};
    const std::string td2{"ATGGAAGTCGCGGAACAAATC"};
    //ATGGAAGTGGCGGAACAAATC

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 1);
}

TEST(Pbmer_Dbg, GetNBreadth_bubble_found_rc)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"GATTTGTTCCGCCACTTCCAT"};
    //ATGGAAGTGGCGGAACAAATC

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 1);
}

TEST(Pbmer_Dbg, GetBubbles_mismatch_found)
{
    const PacBio::Pbmer::Parser parser{15};
    //mismatch                                        (X)
    const std::string td1{"GGCAGTTGATGCTTTAAAGTAATCCAATGTAGAATTCGAATTTTTTTTGT"};
    const std::string td2{"GGCAGTTGATGCTTTAAAGTAATCCAATTTAGAATTCGAATTTTTTTTGT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{15, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 1);
    //reverse comp
    EXPECT_EQ(bubbles[0].LSeq, "AAATTCGAATTCTACATTGGATTACTTTA");
    //reverse comp
    EXPECT_EQ(bubbles[0].RSeq, "AAATTCGAATTCTAAATTGGATTACTTTA");
}
/*
This is de bruijn graph has a complicated topology.Leave as a stretch
        goal.It is probably not possible to recover this event.

    T : TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC(1861 - 1910) ||
    || || || || || || | || || || || || || || || || || || | Q
    : TTCCAGTGCGGGAGG-- ----------CACTGCTAGCATAGCCGGTAATC(1949 - 1986)


TEST(Dbg_GetBubbles, indel_found)
{
    const PacBio::Pbmer::Parser parser{15};
    //del                                (X)
    const std::string td1{"TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC"};
    const std::string td2{"TTCCAGTGCGGGAGGCACTGCTAGCATAGCCGGTAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{15};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    dg.WriteGraph("kmer_graph_indel.dot");

    // this is a failing test
    EXPECT_EQ(bubbles.size(), 2);
}

    */

TEST(Pbmer_Dbg, GetBubbles_indel_found2)
{
    const PacBio::Pbmer::Parser parser{15};
    //del                                  (X)
    const std::string td1{"GTGCCAGCTTCGAAAAAACCTTTAAAAAATAATACATATAAATTTCAACT"};
    const std::string td2{"GTGCCAGCTTCGAAAAACCTTTAAAAAATAATACATATAAATTTCAACT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{15, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 1);
}

TEST(Pbmer_Dbg, GetBubbles_no_bubble)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 0);
}

TEST(Pbmer_Dbg, SpurRemoval_spur_found)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTAGC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto spurCount = dg.RemoveSpurs(1);

    std::string expected = R"(digraph DBGraph {
    ACTTCCA [fillcolor=red, style="rounded,filled", shape=diamond]
    GGAAGTA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    CTACTTC [fillcolor=red, style="rounded,filled", shape=diamond]
    AGCGACT [fillcolor=red, style="rounded,filled", shape=diamond]
    CGACTTC [fillcolor=red, style="rounded,filled", shape=diamond]
    AAGTCGC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    AAGTAGC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    GACTTCC [fillcolor=red, style="rounded,filled", shape=diamond]
    ACTTCCA -> GGAAGTA;
    ACTTCCA -> GACTTCC;
    GGAAGTA -> CTACTTC;
    GGAAGTA -> ACTTCCA;
    CTACTTC -> GGAAGTA;
    CTACTTC -> AAGTAGC;
    AGCGACT -> AAGTCGC;
    CGACTTC -> GACTTCC;
    CGACTTC -> AAGTCGC;
    AAGTCGC -> AGCGACT;
    AAGTCGC -> CGACTTC;
    AAGTAGC -> CTACTTC;
    GACTTCC -> ACTTCCA;
    GACTTCC -> CGACTTC;
})";

    std::string seen = dg.Graph2StringDot();

    EXPECT_EQ(seen, expected);
    EXPECT_EQ(spurCount, 1);
}

TEST(Pbmer_Dbg, SpurRemoval_spur_not_removed)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTAGC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto spurCount = dg.RemoveSpurs(0);

    EXPECT_EQ(spurCount, 0);
}

TEST(Pbmer_Dbg, SpurRemoval_no_spur)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTCGCT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    dg.DumpNodes();

    auto spurCount = dg.RemoveSpurs(5);

    EXPECT_EQ(spurCount, 0);
}

TEST(Pbmer_Dbg, SpurRemoval_spur_bubble_cleanup)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td3{"ATGGAAGTGAAAA"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};
    PacBio::Pbmer::Mers m3{parser.Parse(td3)};

    PacBio::Pbmer::Dbg dg{7, 3};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);

    dg.BuildEdges();

    auto spurCount = dg.RemoveSpurs(1);
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(spurCount, 1);
    EXPECT_EQ(bubbles.size(), 1);
}

TEST(Pbmer_Dbg, GetBubbles_double_bubble_rc)
{
    const std::string td1{
        "AGTCCGGCTTCGGAGCGGGATCAGGAGCGGTGGCCGCTGGGAATACCAATGGCCACAGCACCACCTCCAGTAGCAGCACTCATCTG"
        "AGTC"
        "TGAATGCCAA"};
    const std::string td2{
        "TTGGCATTCAGACTCAGATGAGTACTGCTACTGGAGGTGGTGCTGTGGCCATTGGTATTCCCAGCAGCCACCGCTCCTGATCCCGC"
        "TCCG"
        "AAGCCGGACT"};

    const PacBio::Pbmer::Parser parser{21};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{21, 2};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Pbmer_Dbg, GetBubbles_complex_snv)
{
    const std::string td1{"TGAACAAGTCACAAAAAAAAAAGATAATTTAAATGAAC"};
    const std::string td2{"TGAACAAGTCACAAAAAACAGAGATATTTAAAATGAAC"};
    const std::string td3{"TGAACAAGTCACAAAAAAAAAGATAATTTAAAATGAAC"};
    const std::string td4{"TGAACAAGTCACAAAAACAGAGATAATTTAAAATGAAC"};
    const std::string td5{"TGAACAAGTCACAAAAAAAAAAAGATAATTTAAAATGAAC"};
    const std::string td6{"TGAACAAGTCACAAAAAAAAAAGATAATTTAAAATGAAC"};
    const std::string td7{"TGAACAAGTCACAAAAAACAGAGATAATTTAAAATGAAC"};

    const PacBio::Pbmer::Parser parser{19};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};
    PacBio::Pbmer::Mers m3{parser.Parse(td3)};
    PacBio::Pbmer::Mers m4{parser.Parse(td4)};
    PacBio::Pbmer::Mers m5{parser.Parse(td5)};
    PacBio::Pbmer::Mers m6{parser.Parse(td6)};
    PacBio::Pbmer::Mers m7{parser.Parse(td7)};

    PacBio::Pbmer::Dbg dg{19, 7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);
    dg.AddKmers(m4, 4);
    dg.AddKmers(m5, 5);
    dg.AddKmers(m6, 6);
    dg.AddKmers(m7, 7);

    dg.BuildEdges();

    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 1);
}
