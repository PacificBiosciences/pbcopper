#include <pbcopper/pbmer/Dbg.h>

#include <gtest/gtest.h>

using namespace std::literals;
using PacBio::Pbmer::DnaBit;

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
    EXPECT_EQ(int(m1.forward.size()), 3);
    PacBio::Pbmer::Dbg dg{3, 1};
    dg.AddKmers(m1, 1);

    EXPECT_EQ(dg.NNodes(), 3);

    dg.BuildEdges();

    const std::string expected = R"(digraph DBGraph {
    ATG [fillcolor=red, style="rounded,filled", shape=diamond]
    ATA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    CTA [fillcolor=red, style="rounded,filled", shape=diamond]
    ATG -> ATA;
    ATA -> CTA;
    ATA -> ATG;
    CTA -> ATA;
})";
#if 0
    const std::string oldExpected =
R"(digraph DBGraph {
    ATG [fillcolor=red, style="rounded,filled", shape=diamond]
    CTA [fillcolor=red, style="rounded,filled", shape=diamond]
    ATA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    ATG -> ATA;
    CTA -> ATA;
    ATA -> CTA;
    ATA -> ATG;
})";
#endif

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

    constexpr std::string_view expected{R"(digraph DBGraph {
    ATG [fillcolor=red, style="rounded,filled", shape=diamond]
    AGG [fillcolor=red, style="rounded,filled", shape=diamond]
    ATA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    GTA [fillcolor=red, style="rounded,filled", shape=diamond]
    ACC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    ATG -> ATA;
    AGG -> ACC;
    ATA -> GTA;
    ATA -> ATG;
    GTA -> ATA;
    GTA -> ACC;
    ACC -> AGG;
    ACC -> GTA;
})"};

    const std::string seen = dg.Graph2StringDot();
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

    constexpr std::string_view expected{R"(digraph DBGraph {
    ATG [fillcolor=red, style="rounded,filled", shape=diamond]
    AGG [fillcolor=red, style="rounded,filled", shape=diamond]
    ATA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    GTA [fillcolor=red, style="rounded,filled", shape=diamond]
    ACC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    ATG -> ATA;
    AGG -> ACC;
    ATA -> GTA;
    ATA -> ATG;
    GTA -> ATA;
    GTA -> ACC;
    ACC -> AGG;
    ACC -> GTA;
})"};

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
    auto bubbles = dg.FindBubbles();
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
    auto bubbles = dg.FindBubbles();

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
    auto bubbles = dg.FindBubbles();

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
    auto bubbles = dg.FindBubbles();

    EXPECT_EQ(bubbles.size(), 1);
}

TEST(Pbmer_Dbg, Bubbles_mismatch_found)
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
    auto bubbles = dg.FindBubbles();
    EXPECT_EQ(bubbles.size(), 1);
    const auto& bubble = bubbles.front();
    std::vector<uint64_t> integers{334033404, 11588667, 46354668,  185418674, 741674696,
                                   819215136, 55635075, 64769955,  553063400, 339419382,
                                   283935704, 62000992, 248003971, 64503236,  16125809};
    const std::vector<bool> strands{true, false, false, false, false, false, false, true,
                                    true, false, false, false, false, true,  true};
    std::vector<DnaBit> rightTruth{
        DnaBit{65597948, 1, 15},  DnaBit{11588671, 0, 15},  DnaBit{46354684, 0, 15},
        DnaBit{185418738, 0, 15}, DnaBit{741674952, 0, 15}, DnaBit{819216160, 0, 15},
        DnaBit{55639171, 0, 15},  DnaBit{64753571, 1, 15},  DnaBit{553059304, 1, 15},
        DnaBit{339681526, 0, 15}, DnaBit{284984280, 0, 15}, DnaBit{66195296, 0, 15},
        DnaBit{258012931, 1, 15}, DnaBit{64503232, 1, 15},  DnaBit{16125808, 1, 15}};

    std::vector<DnaBit> leftTruth;
    for (int i = 0, e = integers.size(); i < e; ++i) {
        leftTruth.emplace_back(integers[i], strands[i], 15);
    }
    auto leftPbmers = bubble.LVec;
    auto rightPbmers = bubble.RVec;
    auto standardizeSeq = [&](auto& x) {
        const auto lv = ((__uint128_t(x.front().mer) << 32) | x.front().strand);
        const auto rv = ((__uint128_t(x.back().mer) << 32) | x.back().strand);
        if (lv < rv) {
            std::reverse(x.begin(), x.end());
        }
    };
    standardizeSeq(rightTruth);
    standardizeSeq(leftPbmers);
    standardizeSeq(rightPbmers);
    EXPECT_EQ(std::equal(leftTruth.begin(), leftTruth.end(), leftPbmers.begin()), true);
    EXPECT_EQ(std::equal(rightTruth.begin(), rightTruth.end(), rightPbmers.begin()), true);
}
/*
This is de bruijn graph has a complicated topology.Leave as a stretch
        goal.It is probably not possible to recover this event.

    T : TTCCAGTGCGGGAGGAGGAGGAGGAGGCACTGCTAGCATAGCCGGTAATC(1861 - 1910) ||
    || || || || || || | || || || || || || || || || || || | Q
    : TTCCAGTGCGGGAGG-- ----------CACTGCTAGCATAGCCGGTAATC(1949 - 1986)


TEST(Dbg_Bubbles, indel_found)
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
    auto bubbles = dg.FindBubbles();
    dg.WriteGraph("kmer_graph_indel.dot");

    // this is a failing test
    EXPECT_EQ(bubbles.size(), 2);
}

    */

TEST(Pbmer_Dbg, Bubbles_indel_found2)
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
    auto bubbles = dg.FindBubbles();
    const std::ptrdiff_t lSize = bubbles.front().LSeq.size();
    const std::ptrdiff_t rSize = bubbles.front().RSeq.size();
    const std::string& lseq = bubbles.front().LSeq;
    const std::string& rseq = bubbles.front().RSeq;

    EXPECT_EQ(bubbles.size(), 1);
    // There should be an insertion of one A.
    EXPECT_EQ(std::max(lSize, rSize) - std::min(lSize, rSize), 1);

    // Make sure that the strings don't change. This may change with hash-map ordering
    // so if only these tests fail after changing the underlying hash-map, it may not be bad news.
    EXPECT_EQ(lseq, "CCAGCTTCGAAAAACCTTTAAAA");
    EXPECT_EQ(rseq, "CCAGCTTCGAAAAAACCTTTAAAA");
    EXPECT_EQ(rseq.substr(0, 14) + rseq.substr(15, std::string::npos), lseq);
}

TEST(Pbmer_Dbg, Bubbles_no_bubble)
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

    auto bubbles = dg.FindBubbles();

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

    constexpr std::string_view expected{R"(digraph DBGraph {
    AGCGACT [fillcolor=red, style="rounded,filled", shape=diamond]
    AAGTAGC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    GACTTCC [fillcolor=red, style="rounded,filled", shape=diamond]
    GGAAGTA [fillcolor=grey, style="rounded,filled", shape=ellipse]
    CGACTTC [fillcolor=red, style="rounded,filled", shape=diamond]
    AAGTCGC [fillcolor=grey, style="rounded,filled", shape=ellipse]
    ACTTCCA [fillcolor=red, style="rounded,filled", shape=diamond]
    CTACTTC [fillcolor=red, style="rounded,filled", shape=diamond]
    AGCGACT -> AAGTCGC;
    AAGTAGC -> CTACTTC;
    GACTTCC -> ACTTCCA;
    GACTTCC -> CGACTTC;
    GGAAGTA -> CTACTTC;
    GGAAGTA -> ACTTCCA;
    CGACTTC -> GACTTCC;
    CGACTTC -> AAGTCGC;
    AAGTCGC -> AGCGACT;
    AAGTCGC -> CGACTTC;
    ACTTCCA -> GGAAGTA;
    ACTTCCA -> GACTTCC;
    CTACTTC -> GGAAGTA;
    CTACTTC -> AAGTAGC;
})"};

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
    auto bubbles = dg.FindBubbles();

    EXPECT_EQ(spurCount, 1);
    EXPECT_EQ(bubbles.size(), 1);
}

TEST(Pbmer_Dbg, Bubbles_double_bubble_rc)
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

    auto bubbles = dg.FindBubbles();

    EXPECT_EQ(bubbles.size(), 2);
}

TEST(Pbmer_Dbg, Bubbles_complex_snv)
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

    auto bubbles = dg.FindBubbles();

    EXPECT_EQ(bubbles.size(), 1);
}
