#include <array>
#include <memory>

#include <gtest/gtest.h>

#include <pbcopper/dagcon/Alignment.h>
#include <pbcopper/dagcon/AlignmentGraph.h>

using namespace PacBio;

// clang-format off

TEST(Dagcon_AlignmentGraph, can_generate_raw_consensus)
{
    std::array<Dagcon2::Alignment, 5> algs;

    algs[0].Target = "ATATTA---GGC";
    algs[0].Query  = "ATAT-AGCCGGC";

    algs[1].Target = "ATATTA-GGC";
    algs[1].Query  = "ATAT-ACGGC";

    algs[2].Target = "AT-ATTA--GGC";
    algs[2].Query  = "ATCAT--CCGGC";

    algs[3].Target = "ATATTA--G-GC";
    algs[3].Query  = "ATAT-ACCGAG-";

    algs[4].Target = "ATATTA---GGC";
    algs[4].Query  = "ATAT-AGCCGGC";

    for(int i=0; i < 5; i++) {
        auto& ra = algs[i];
        ra.Id = "target";
        ra.TargetLength = 9;
        ra.Start = 1;
    }

    const std::string backbone{"ATATTAGGC"};
    Dagcon2::AlignmentGraph ag{backbone};
    ag.AddAlignment(algs[0]);
    ag.AddAlignment(algs[1]);
    ag.AddAlignment(algs[2]);
    ag.AddAlignment(algs[3]);
    ag.AddAlignment(algs[4]);
    ag.MergeNodes();

    const std::string expected = "ATATAGCCGGC";
    const std::string actual = ag.Consensus();
    EXPECT_EQ(expected, actual);
}

TEST(Dagcon_AlignmentGraph, dangling_nodes)
{
    Dagcon2::Alignment a;
    a.Target = "C-GCGGA-T-G-";
    a.Query  = "CCGCGG-G-A-T";

    Dagcon2::AlignmentGraph ag{12};
    ag.AddAlignment(a);
    EXPECT_FALSE(ag.DanglingNodes());
}

// clang-format on
