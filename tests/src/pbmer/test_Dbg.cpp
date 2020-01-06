#include <gtest/gtest.h>

#include <pbcopper/pbmer/Dbg.h>

TEST(Dbg_AddKmers, check_throw_kmer_too_big)
{
    const PacBio::Pbmer::Parser parser{31};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{31};

    EXPECT_EQ(dg.AddKmers(m, 1), -1);
}

TEST(Dbg_AddKmers, check_throw_wrong_width)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{16};

    EXPECT_EQ(dg.AddKmers(m, 1), -2);
}

/*
TEST(Dbg_Dump, check_dump)
{
    const PacBio::Pbmer::Parser parser{5};
    const std::string td1{"CATACGTCCGTG"};
    const std::string td2{"CATACGTCCGTG"};
    const std::string td3{"CATACGTCCGTG"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};

    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Mers m3{parser.Parse(td3)};

    PacBio::Pbmer::Dbg dg{5};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    //dg.AddKmers(m3, 3);

    dg.BuildEdges();
    dg.WriteGraph("kmer_graph.dot");
    EXPECT_EQ(1, 1);
}
*/

TEST(Dbg_ValidateLoad, check_load)
{
    const PacBio::Pbmer::Parser parser{3};
    const std::string td1{"ATCGCT"};
    const std::string td2{"ATCGAT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{3};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    EXPECT_TRUE(dg.ValidateLoad());
}

TEST(Dbg_ValidateEdges, check_edge)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    //dg.WriteGraph("kmer_graph.dot");
    EXPECT_TRUE(dg.ValidateEdges());
}

TEST(Dbg_FrequencyFilterNodes, empty)
{

    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    dg.FrequencyFilterNodes(10);
    EXPECT_EQ(dg.NNodes(), 0);
}

TEST(Dbg_FrequencyFilterNodes, one_left)
{

    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td3{"ATGGAAG"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};
    PacBio::Pbmer::Mers m3{parser.Parse(td3)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);

    dg.BuildEdges();
    dg.FrequencyFilterNodes(3);
    EXPECT_EQ(dg.NNodes(), 1);
}

TEST(Dbg_GetNBreadth, bubble_found)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    dg.WriteGraph("bubble_forward.dot");

    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetNBreadth, node_count_correct)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"GATTTGTTCCGCGACTTCCAT"};
    //ATGGAAGTGGCGGAACAAATC

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(dg.NNodes(), 15);
    EXPECT_EQ(bubbles.size(), 0);
}

TEST(Dbg_GetNBreadth, bubble_found_rc_first)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"GATTTGTTCCGCCACTTCCAT"};
    const std::string td2{"ATGGAAGTCGCGGAACAAATC"};
    //ATGGAAGTGGCGGAACAAATC

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    std::cerr << "INFO " << dg.NNodes() << "\n";
    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    dg.WriteGraph("bubble_rc_first.dot");

    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetNBreadth, bubble_found_rc)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"GATTTGTTCCGCCACTTCCAT"};
    //ATGGAAGTGGCGGAACAAATC

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    std::cerr << "INFO " << dg.NNodes() << "\n";
    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    dg.WriteGraph("bubble_rc.dot");

    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetBubbles, mismatch_found)
{
    const PacBio::Pbmer::Parser parser{15};
    //mismatch                                        (X)
    const std::string td1{"GGCAGTTGATGCTTTAAAGTAATCCAATGTAGAATTCGAATTTTTTTTGT"};
    const std::string td2{"GGCAGTTGATGCTTTAAAGTAATCCAATTTAGAATTCGAATTTTTTTTGT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{15};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    //dg.WriteGraph("kmer_graph_mismatch.dot");

    EXPECT_EQ(bubbles.size(), 2);
}

/*
This is de bruijn graph has a complicated topology. Leave as a stretch goal.


T: TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC (1861 - 1910)
   |||||||||||||||            |||||||||||||||||||||||
Q: TTCCAGTGCGGGAGG------------CACTGCTAGCATAGCCGGTAATC (1949 - 1986)


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

TEST(Dbg_GetBubbles, indel_found2)
{
    const PacBio::Pbmer::Parser parser{15};
    //del                                  (X)
    const std::string td1{"GTGCCAGCTTCGAAAAAACCTTTAAAAAATAATACATATAAATTTCAACT"};
    const std::string td2{"GTGCCAGCTTCGAAAAACCTTTAAAAAATAATACATATAAATTTCAACT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{15};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto bubbles = dg.GetBubbles();
    dg.WriteGraph("kmer_graph_indel2.dot");

    // this is a failing test
    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetBubbles, no_bubble)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 0);
}

TEST(Dbg_SpurRemoval, spur_found)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTAGC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto spurCount = dg.RemoveSpurs(10);

    EXPECT_EQ(spurCount, 1);
}

TEST(Dbg_SpurRemoval, spur_not_removed)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTAGC"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto spurCount = dg.RemoveSpurs(0);

    EXPECT_EQ(spurCount, 0);
}

TEST(Dbg_SpurRemoval, no_spur)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTCGCT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();
    auto spurCount = dg.RemoveSpurs(10);

    EXPECT_EQ(spurCount, 0);
}

TEST(Dbg_SpurRemoval, spur_bubble_cleanup)
{
    const PacBio::Pbmer::Parser parser{7};
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td3{"ATGGAAGTGA"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};
    PacBio::Pbmer::Mers m3{parser.Parse(td3)};

    PacBio::Pbmer::Dbg dg{7};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);

    dg.BuildEdges();
    // dg.WriteGraph("spur_bubble.dot");

    auto spurCount = dg.RemoveSpurs(1);
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(spurCount, 1);
    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetBubbles, double_bubble_rc)
{
    const std::string td1{
        "AGTCCGGCTTCGGAGCGGGATCAGGAGCGGTGGCCGCTGGGAATACCAATGGCCACAGCACCACCTCCAGTAGCAGCACTCATCTGAGTC"
        "TGAATGCCAA"};
    const std::string td2{
        "TTGGCATTCAGACTCAGATGAGTACTGCTACTGGAGGTGGTGCTGTGGCCATTGGTATTCCCAGCAGCCACCGCTCCTGATCCCGCTCCG"
        "AAGCCGGACT"};

    const PacBio::Pbmer::Parser parser{21};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{21};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);

    dg.BuildEdges();

    dg.WriteGraph("odd_topo.dot");
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 4);
}

TEST(Dbg_GetBubbles, complex_snv)
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

    PacBio::Pbmer::Dbg dg{19};

    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);
    dg.AddKmers(m4, 4);
    dg.AddKmers(m5, 5);
    dg.AddKmers(m6, 6);
    dg.AddKmers(m7, 7);

    dg.BuildEdges();

    dg.WriteGraph("complex_snv.dot");
    auto bubbles = dg.GetBubbles();

    EXPECT_EQ(bubbles.size(), 2);
}

TEST(DbgNode, can_iterate_over_dnabits)
{
    // Zev, test this. Didn't want to start making everything public
    // for testing. You can list expected values better than I can.
}
