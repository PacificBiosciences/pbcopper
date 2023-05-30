#include <pbcopper/poa/PoaConsensus.h>

#include <cstdlib>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/assign/std/vector.hpp>

#include <pbcopper/align/AlignConfig.h>

#include <gtest/gtest.h>

using boost::erase_all_copy;

using namespace boost::assign;  // NOLINT
using namespace PacBio::Poa;    // NOLINT
using namespace PacBio::Align;  // NOLINT

#define MAKE_ALL_PLOTS false

namespace PoaConsensusTests {
namespace {

void plotConsensus(const PacBio::Poa::PoaConsensus* pc, std::string description,
                   bool REALLY_MAKE_THIS_ONE = false)
{
    if (MAKE_ALL_PLOTS || REALLY_MAKE_THIS_ONE) {
        std::string dotFname = description + ".dot";
        std::string pngFname = description + ".png";
        std::string cmd = std::string("dot -Tpng ") + dotFname + " > " + pngFname;
        pc->Graph.WriteGraphVizFile(description + ".dot",
                                    (PoaGraph::COLOR_NODES | PoaGraph::VERBOSE_NODES), pc);
        // std::cout << cmd << std::endl;
        const int result = std::system(cmd.c_str());
        if (result) {
            std::cerr << "Running '" << cmd << "' failed\n";
        }
    }
}

}  // namespace
}  // namespace PoaConsensusTests

// TEST(PoaGraph, NoReadsTest)
// {
//  // Test that it works with no reads
//  std::vector<const SequenceFeatures*> reads;
//  std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{PoaConsensus::findConsensus(reads,
//  PoaConfig::GLOBAL_ALIGNMENT)};
//  std::string dot = pc->getGraph()->toGraphViz();
//  std::cout << dot << std::endl;
// }

TEST(PoaGraph, SmallBasicTest1)
{
    // Test that it works with a single sequence
    std::vector<std::string> reads;
    reads += "GGG";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
    std::string dot = pc->Graph.ToGraphViz();
    std::string expectedDot =
        "digraph G {"
        "rankdir=\"LR\";"
        "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
        "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
        "2[shape=Mrecord, label=\"{ G | 1 }\"];"
        "3[shape=Mrecord, label=\"{ G | 1 }\"];"
        "4[shape=Mrecord, label=\"{ G | 1 }\"];"
        "0->2 ;"
        "2->3 ;"
        "3->4 ;"
        "4->1 ;"
        "}";
    PoaConsensusTests::plotConsensus(pc.get(), "small-basic-1");
    EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
    EXPECT_EQ("GGG", pc->Sequence);
}

TEST(PoaGraph, SmallBasicTest2)
{
    // Test that it works with two identical sequences
    std::vector<std::string> reads;
    reads += "GGG", "GGG";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
    std::string dot = pc->Graph.ToGraphViz();
    std::string expectedDot =
        "digraph G {"
        "rankdir=\"LR\";"
        "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
        "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
        "2[shape=Mrecord, label=\"{ G | 2 }\"];"
        "3[shape=Mrecord, label=\"{ G | 2 }\"];"
        "4[shape=Mrecord, label=\"{ G | 2 }\"];"
        "0->2 ;"
        "2->3 ;"
        "3->4 ;"
        "4->1 ;"
        "}";
    PoaConsensusTests::plotConsensus(pc.get(), "small-basic-2");
    EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
    EXPECT_EQ("GGG", pc->Sequence);
}

TEST(PoaGraph, SmallExtraTests)
{
    // Extra at beginning
    {
        std::vector<std::string> reads;
        reads += "GGG", "TGGG";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 2 }\"];"
            "3[shape=Mrecord, label=\"{ G | 2 }\"];"
            "4[shape=Mrecord, label=\"{ G | 2 }\"];"
            "5[shape=Mrecord, label=\"{ T | 1 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "5->2 ;"
            "0->5 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "extra-at-beginning");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("GGG", pc->Sequence);
    }

    // Extra in middle
    {
        std::vector<std::string> reads;
        reads += "GGG", "GTGG";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        // this would be easier if we could use the C++0x raw
        // strings feature (in g++ 4.5+)
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 2 }\"];"
            "3[shape=Mrecord, label=\"{ G | 2 }\"];"
            "4[shape=Mrecord, label=\"{ G | 2 }\"];"
            "5[shape=Mrecord, label=\"{ T | 1 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "5->3 ;"
            "2->5 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "extra-in-middle");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("GGG", pc->Sequence);
    }

    // Extra at end
    {
        std::vector<std::string> reads;
        reads += "GGG", "GGGT";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 2 }\"];"
            "3[shape=Mrecord, label=\"{ G | 2 }\"];"
            "4[shape=Mrecord, label=\"{ G | 2 }\"];"
            "5[shape=Mrecord, label=\"{ T | 1 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "5->1 ;"
            "4->5 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "extra-at-end");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("GGG", pc->Sequence);
    }
}

TEST(PoaGraph, SmallMismatchTests)
{
    // Mismatch at beginning
    {
        std::vector<std::string> reads;
        reads += "GGG", "TGG";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 1 }\"];"
            "3[shape=Mrecord, label=\"{ G | 2 }\"];"
            "4[shape=Mrecord, label=\"{ G | 2 }\"];"
            "5[shape=Mrecord, label=\"{ T | 1 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "5->3 ;"
            "0->5 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "mismatch-at-beginning");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("GG", pc->Sequence);
    }

    // Mismatch in middle
    {
        std::vector<std::string> reads;
        reads += "GGG", "GTG", "GTG";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 3 }\"];"
            "3[shape=Mrecord, label=\"{ G | 1 }\"];"
            "4[shape=Mrecord, label=\"{ G | 3 }\"];"
            "5[shape=Mrecord, label=\"{ T | 2 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "5->4 ;"
            "2->5 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "mismatch-in-middle");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("GTG", pc->Sequence);
    }

    // Mismatch at end
    {
        std::vector<std::string> reads;
        reads += "GGG", "GGT";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 2 }\"];"
            "3[shape=Mrecord, label=\"{ G | 2 }\"];"
            "4[shape=Mrecord, label=\"{ G | 1 }\"];"
            "5[shape=Mrecord, label=\"{ T | 1 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "5->1 ;"
            "3->5 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "mismatch-at-end");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("GG", pc->Sequence);
    }
}

TEST(PoaGraph, SmallDeletionTests)
{
    // Deletion at beginning
    {
        std::vector<std::string> reads;
        reads += "GAT", "AT";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 1 }\"];"
            "3[shape=Mrecord, label=\"{ A | 2 }\"];"
            "4[shape=Mrecord, label=\"{ T | 2 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "0->3 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "deletion-at-beginning");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("AT", pc->Sequence);
    }

    // Deletion in middle
    {
        std::vector<std::string> reads;
        reads += "GAT", "GT";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 2 }\"];"
            "3[shape=Mrecord, label=\"{ A | 1 }\"];"
            "4[shape=Mrecord, label=\"{ T | 2 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "2->4 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "deletion-in-middle");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
    }

    // Deletion at end
    {
        std::vector<std::string> reads;
        reads += "GAT", "GA";
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
        std::string dot = pc->Graph.ToGraphViz();
        std::string expectedDot =
            "digraph G {"
            "rankdir=\"LR\";"
            "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
            "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
            "2[shape=Mrecord, label=\"{ G | 2 }\"];"
            "3[shape=Mrecord, label=\"{ A | 2 }\"];"
            "4[shape=Mrecord, label=\"{ T | 1 }\"];"
            "0->2 ;"
            "2->3 ;"
            "3->4 ;"
            "4->1 ;"
            "3->1 ;"
            "}";
        PoaConsensusTests::plotConsensus(pc.get(), "deletion-at-end");
        EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
        EXPECT_EQ("GA", pc->Sequence);
    }
}

TEST(PoaConsensus, TestSimple)
{
    std::vector<std::string> reads;
    reads += "TTTACAGGATAGTCCAGT", "ACAGGATACCCCGTCCAGT", "ACAGGATAGTCCAGT",
        "TTTACAGGATAGTCCAGTCCCC", "TTTACAGGATTAGTCCAGT", "TTTACAGGATTAGGTCCCAGT",
        "TTTACAGGATAGTCCAGT";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
    PoaConsensusTests::plotConsensus(pc.get(), "simple");
    EXPECT_EQ("TTTACAGGATAGTCCAGT", pc->Sequence);
}

TEST(PoaConsensus, TestOverhangSecond)
{
    std::vector<std::string> reads;
    reads += "TTTACAGGATAGTCCAGT", "TTTACAGGATAGTCCAGTAAA", "TTTACAGGATAGTCCAGTAAA";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
    EXPECT_EQ("TTTACAGGATAGTCCAGTAAA", pc->Sequence);
}

TEST(PoaConsensus, SmallSemiglobalTest)
{
    std::vector<std::string> reads;
    reads += "GGTGG", "GGTGG", "T";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::SEMIGLOBAL)};
    PoaConsensusTests::plotConsensus(pc.get(), "small-semiglobal");
    std::string expectedDot =
        "digraph G {"
        "rankdir=\"LR\";"
        "0[shape=Mrecord, label=\"{ ^ | 0 }\"];"
        "1[shape=Mrecord, label=\"{ $ | 0 }\"];"
        "2[shape=Mrecord, label=\"{ G | 2 }\"];"
        "3[shape=Mrecord, label=\"{ G | 2 }\"];"
        "4[shape=Mrecord, label=\"{ T | 3 }\"];"
        "5[shape=Mrecord, label=\"{ G | 2 }\"];"
        "6[shape=Mrecord, label=\"{ G | 2 }\"];"
        "0->2 ;"
        "2->3 ;"
        "3->4 ;"
        "4->5 ;"
        "5->6 ;"
        "6->1 ;"
        "4->1 ;"
        "0->4 ;"
        "}";
    std::string dot = pc->Graph.ToGraphViz();
    EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
    EXPECT_EQ("GGTGG", pc->Sequence);
}

TEST(PoaConsensus, SmallTilingTest)
{
    std::vector<std::string> reads;
    reads += "GGGGAAAA", "AAAATTTT", "TTTTCCCC", "CCCCAGGA";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::SEMIGLOBAL)};
    PoaConsensusTests::plotConsensus(pc.get(), "small-tiling");
    EXPECT_EQ("GGGGAAAATTTTCCCCAGGA", pc->Sequence);
}

TEST(PoaConsensus, TestVerboseGraphVizOutput)
{
    std::vector<std::string> reads;
    reads += "GGG", "TGGG";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
    std::string dot =
        pc->Graph.ToGraphViz(PoaGraph::COLOR_NODES | PoaGraph::VERBOSE_NODES, pc.get());

    std::string expectedDot =
        "digraph G {"
        "rankdir=\"LR\";"
        "0[shape=Mrecord, label=\"{ { 0 | ^ } | { 0 | 0 } | { 0.00 | 0.00 } "
        "}\"];"
        "1[shape=Mrecord, label=\"{ { 1 | $ } | { 0 | 0 } | { 0.00 | 0.00 } "
        "}\"];"
        "2[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" ,"
        " label=\"{ { 2 | G } | { 2 | 2 } | { 2.00 | 2.00 } }\"];"
        "3[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" ,"
        " label=\"{ { 3 | G } | { 2 | 2 } | { 2.00 | 4.00 } }\"];"
        "4[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" ,"
        " label=\"{ { 4 | G } | { 2 | 2 } | { 2.00 | 6.00 } }\"];"
        "5[shape=Mrecord, label=\"{ { 5 | T } | { 1 | 2 } | { -0.00 | -0.00 } "
        "}\"];"
        "0->2 ;"
        "2->3 ;"
        "3->4 ;"
        "4->1 ;"
        "5->2 ;"
        "0->5 ;}";

    EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
}

TEST(PoaConsensus, TestLocalStaggered)
{
    // Adapted from Pat's C# test
    std::vector<std::string> reads;
    reads += "TTTACAGGATAGTGCCGCCAATCTTCCAGT", "GATACCCCGTGCCGCCAATCTTCCAGTATATACAGCACGAGTAGC",
        "ATAGTGCCGCCAATCTTCCAGTATATACAGCACGGAGTAGCATCACGTACGTACGTCTACACGTAATT",
        "ACGTCTACACGTAATTTTGGAGAGCCCTCTCTCACG", "ACACGTAATTTTGGAGAGCCCTCTCTTCACG",
        "AGGATAGTGCCGCCAATCTTCCAGTAATATACAGCACGGAGTAGCATCACGTACG",
        "ATAGTGCCGCCAATCTTCCAGTATATACAGCACGGAGTAGCATCACGTACGTACGTCTACACGT";

    // 4 is a magic number here.  the .NET code had an entrenched
    // assumption that sequences in POA were subreads from a ZMW, so
    // the minCoverage was (numReads - 3), under assumption that basal
    // coverage for CCS is (numReads-2) (beginning, end read).
    // Application has to provide a sensible minCoverage.
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::LOCAL, 4)};
    PoaConsensusTests::plotConsensus(pc.get(), "local-staggered", false);
    EXPECT_EQ("ATAGTGCCGCCAATCTTCCAGTATATACAGCACGGAGTAGCATCACGTACGTACGTCTACACGTAATT", pc->Sequence);
}

TEST(PoaConsensus, TestLongInsert)
{
    // Adapted from Pat's C# test
    std::vector<std::string> reads;
    reads +=
        "TTTACAGGATAGTGCCGCCAATCTTCCAGTGATACCCCGTGCCGCCAATCTTCCAGTATATACAGCACGA"
        "GGTAGC",
        "TTTACAGGATAGTGCCGGCCAATCTTCCAGTGATACCCCGTGCCGCCAATCTTCCAGTATATACAGCACG"
        "AGTAGC",
        "TTGTACAGGATAGTGCCGCCAATCTTCCAGTGATGGGGGGGGGGGGGGGGGGGGGGGGGGGACCCCGTGC"
        "CGCCAATCTTCCAGTATATACAGCACGAGTAGC";
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
    EXPECT_EQ(
        "TTTACAGGATAGTGCCGCCAATCTTCCAGTGATACCCCGTGCCGCCAATCTTCCAGTATATACAGCACGA"
        "GTAGC",
        pc->Sequence);
}

TEST(PoaConsensus, TestSpanningReads)
{
    std::string read1 = "GAAAG";
    std::string read2 = "GATAG";
    std::vector<std::string> reads{read1, read1, read1, read2, read2, read2};
    std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
        PoaConsensus::FindConsensus(reads, AlignMode::LOCAL)};
    PoaConsensusTests::plotConsensus(pc.get(), "spanning-reads");

    std::string dot =
        pc->Graph.ToGraphViz(PoaGraph::VERBOSE_NODES | PoaGraph::COLOR_NODES, pc.get());
    // We expect to get spanning reads of 6 for the middle A/T nodes,
    // but each only has 3 reads passing through.
    // The PoaGraph doesn't really expose an API, we can only check it
    // by looking at the GraphViz output.

    // clang-format off
    std::string expectedDot =
        "digraph G {"
        "rankdir=\"LR\";"
        "0[shape=Mrecord, label=\"{ { 0 | ^ } | { 0 | 0 } | { 0.00 | 0.00 } }\"];"
        "1[shape=Mrecord, label=\"{ { 1 | $ } | { 0 | 0 } | { 0.00 | 0.00 } }\"];"
        "2[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" , label=\"{ { 2 | G } | { 6 | 6 } | { 6.00 | 6.00 } }\"];"
        "3[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" , label=\"{ { 3 | A } | { 6 | 6 } | { 6.00 | 12.00 } }\"];"
        "4[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" , label=\"{ { 4 | A } | { 3 | 6 } | { -0.00 | 12.00 } }\"];"
        "5[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" , label=\"{ { 5 | A } | { 6 | 6 } | { 6.00 | 18.00 } }\"];"
        "6[shape=Mrecord, style=\"filled\", fillcolor=\"lightblue\" , label=\"{ { 6 | G } | { 6 | 6 } | { 6.00 | 24.00 } }\"];"
        "7[shape=Mrecord, label=\"{ { 7 | T } | { 3 | 6 } | { -0.00 | 12.00 } }\"];"
        "0->2 ;"
        "2->3 ;"
        "3->4 ;"
        "4->5 ;"
        "5->6 ;"
        "6->1 ;"
        "7->5 ;"
        "3->7 ;"
        "}";
    // clang-format on

    EXPECT_EQ(expectedDot, erase_all_copy(dot, "\n"));
}

TEST(PoaConsensus, NondeterminismRegressionTest)
{
    //
    // This is a regression test for a real-world case of
    // nondeterminism found in the POA on a quiver job on Staph.
    //
    std::vector<std::string> reads;
    reads +=
        "TATCAATCAACGAAATTCGCCAATTCCGTCATGAATGTCAATATCTAACTACACTTTAGAATACATTCTT"
        "TGACATGCCTGGCCTATTGATATTTCAATAAAATCAGACTATAAAGACAACTTACAAATGATCCTATAAA"
        "TTAAAGATCGAGAATCTAAAGAGTGAAATTAAAGCTAATTACTGCTTTAAAAATTTTACGTGCACACAAA"
        "AATGAATTTATCCTCATTATATCGAAAATACCATGAAGTATAGTAAGCTAACTTGAATATGATCATTAAT"
        "CGGCTATATGATTATTTTGATAATGCAATGAGCATCAATCTGAATTTATGACCTATCATTCGCGTTGCAT"
        "TTATTGAAGTGAAAATTCATGTACGCTTTTTTATTTTATTAATATAATCCTTGATATTGGTTATATACCA"
        "CGCTGTCACATAATTTTCAATAAATTTTTCTACTAAATGAAGTGTCTGTTATCTATCAC";
    reads +=
        "TATCAACAACGAAAATGCGCAGTTACGTCATGATTTATGTCAAATAATCTAAACGACACTTTCAGAAATA"
        "AATACATTCGAGAAGATGAATGCCTGGCGCAAAGTGATTATTTCAATAAAATATTTGTACCTTGAAAGAC"
        "AATTTACAAATGAATGCTATAAAATTTAAATGGATCCGGAGAATCTTTAAAGTACGTGAAATTAAAGGCT"
        "AAGATTACTGCGAAAAATTTTCGTGCACAAGAAATGAATGTTCCAGATTAGTATCGGAAAATAAGCCATG"
        "AAGAAGCTAGCATTAACTTGAATATGATCGATTTAATCGGCAGTATTGGTAATTATCTTGATAAGCAATT"
        "GAGCATCAACTGAAATTGAATGACTCTACATGCCTCGCTGAGTATGCGATTTATTGAAAGTGAAATTCAG"
        "TAAAGTTTATTGTTATGAATAAATGCGTACTTGGATGAATATCCCGACGGTAGTTCAAGTGTAAATGGAG"
        "TGAGGGGGTTCTTTCTTATAGAATAGTTTTATACTACTGATAAGGTGTAACCTGAGTGAGTCGTGATTTT"
        "AGAGTTACTTGCGAAC";

    std::set<std::string> answers;
    for (int run = 0; run < 100; run++) {
        std::unique_ptr<const PacBio::Poa::PoaConsensus> pc{
            PoaConsensus::FindConsensus(reads, AlignMode::GLOBAL)};
#if 0
        char fname[100];
        std::sprintf(fname, "/tmp/gr%03d.dot", run);
        pc->WriteGraphVizFile(fname, (PoaGraph::VERBOSE_NODES | PoaGraph::COLOR_NODES));
#endif
        answers.insert(pc->Sequence);
    }
    ASSERT_EQ(1, answers.size());
}
