#include <gtest/gtest.h>

#include <pbcopper/pbmer/Dbg.h>

TEST(Dbg_AddKmers, check_throw_kmer_too_big)
{
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};

    const PacBio::Pbmer::Parser parser{31};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    PacBio::Pbmer::Dbg dg{31};
    EXPECT_EQ(dg.AddKmers(m, 1), -1);
}

TEST(Dbg_AddKmers, check_throw_wrong_width)
{
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};

    const PacBio::Pbmer::Parser parser{16};
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
    const std::string td1{"ATCGCT"};
    const std::string td2{"ATCGAT"};

    const PacBio::Pbmer::Parser parser{3};
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
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    EXPECT_TRUE(dg.ValidateEdges());
}

TEST(Dbg_FrequencyFilterNodes, empty)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    const PacBio::Pbmer::Parser parser{7};
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
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td3{"ATGGAAG"};

    const PacBio::Pbmer::Parser parser{7};
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
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetNBreadth, node_count_correct)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"GATTTGTTCCGCGACTTCCAT"};
    //ATGGAAGTGGCGGAACAAATC

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto bubbles = dg.GetBubbles();
    EXPECT_EQ(dg.NNodes(), 15);
    EXPECT_EQ(bubbles.size(), 0);
}

TEST(Dbg_GetNBreadth, bubble_found_rc_first)
{
    //SNV                         (X)
    const std::string td1{"GATTTGTTCCGCCACTTCCAT"};
    const std::string td2{"ATGGAAGTCGCGGAACAAATC"};
    //ATGGAAGTGGCGGAACAAATC

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetNBreadth, bubble_found_rc)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"GATTTGTTCCGCCACTTCCAT"};
    //ATGGAAGTGGCGGAACAAATC

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetBubbles, mismatch_found)
{
    //mismatch                                        (X)
    const std::string td1{"GGCAGTTGATGCTTTAAAGTAATCCAATGTAGAATTCGAATTTTTTTTGT"};
    const std::string td2{"GGCAGTTGATGCTTTAAAGTAATCCAATTTAGAATTCGAATTTTTTTTGT"};

    const PacBio::Pbmer::Parser parser{15};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{15};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto bubbles = dg.GetBubbles();
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
    //del                                  (X)
    const std::string td1{"GTGCCAGCTTCGAAAAAACCTTTAAAAAATAATACATATAAATTTCAACT"};
    const std::string td2{"GTGCCAGCTTCGAAAAACCTTTAAAAAATAATACATATAAATTTCAACT"};

    const PacBio::Pbmer::Parser parser{15};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{15};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    // this is a failing test
    const auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Dbg_GetBubbles, no_bubble)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 0);
}

TEST(Dbg_SpurRemoval, spur_found)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTAGC"};

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto spurCount = dg.RemoveSpurs(10);
    EXPECT_EQ(spurCount, 1);
}

TEST(Dbg_SpurRemoval, spur_not_removed)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTAGC"};

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto spurCount = dg.RemoveSpurs(0);
    EXPECT_EQ(spurCount, 0);
}

TEST(Dbg_SpurRemoval, no_spur)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCT"};
    const std::string td2{"ATGGAAGTCGCT"};

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.BuildEdges();

    const auto spurCount = dg.RemoveSpurs(10);
    EXPECT_EQ(spurCount, 0);
}

TEST(Dbg_SpurRemoval, spur_bubble_cleanup)
{
    //SNV                         (X)
    const std::string td1{"ATGGAAGTCGCGGAACAAATC"};
    const std::string td2{"ATGGAAGTGGCGGAACAAATC"};
    const std::string td3{"ATGGAAGTGA"};

    const PacBio::Pbmer::Parser parser{7};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};
    PacBio::Pbmer::Mers m3{parser.Parse(td3)};

    PacBio::Pbmer::Dbg dg{7};
    dg.AddKmers(m1, 1);
    dg.AddKmers(m2, 2);
    dg.AddKmers(m3, 3);
    dg.BuildEdges();

    const auto spurCount = dg.RemoveSpurs(1);
    const auto bubbles = dg.GetBubbles();
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

    const auto bubbles = dg.GetBubbles();
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

    const auto bubbles = dg.GetBubbles();
    EXPECT_EQ(bubbles.size(), 2);
}

TEST(DbgNode, can_iterate_over_dnabits)
{
    // Zev, test this. Didn't want to start making everything public
    // for testing. You can list expected values better than I can.
}
