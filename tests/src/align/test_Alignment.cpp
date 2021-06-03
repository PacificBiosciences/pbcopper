#include <pbcopper/align/AffineAlignment.h>
#include <pbcopper/align/AlignConfig.h>
#include <pbcopper/align/LinearAlignment.h>
#include <pbcopper/align/LocalAlignment.h>
#include <pbcopper/align/PairwiseAlignment.h>

#include <algorithm>
#include <memory>

#include <gtest/gtest.h>

namespace PacBio {
namespace Align {
namespace internal {

bool Rewrite2L(std::string* target, std::string* query, std::string* transcript, size_t i);
bool Rewrite3L(std::string* target, std::string* query, std::string* transcript, size_t i);
bool Rewrite2R(std::string* target, std::string* query, std::string* transcript, size_t i);
bool Rewrite3R(std::string* target, std::string* query, std::string* transcript, size_t i);

}  // namespace internal
}  // namespace Align
}  // namespace PacBio

using PairwiseAlignment = PacBio::Align::PairwiseAlignment;  // NOLINT

TEST(Align_PairwiseAlignment, calculates_metrics_from_input_aligned_sequences)
{
    const PairwiseAlignment a{"GATC", "GA-C"};
    EXPECT_EQ("GATC", a.Target());
    EXPECT_EQ("GA-C", a.Query());
    EXPECT_EQ(4, a.Length());
    EXPECT_EQ(3, a.Matches());
    EXPECT_EQ(1, a.Deletions());
    EXPECT_EQ(0, a.Mismatches());
    EXPECT_EQ(0, a.Insertions());
    EXPECT_FLOAT_EQ(0.75, a.Accuracy());
    EXPECT_EQ("MMDM", a.Transcript());

    const PairwiseAlignment a2{"GATTA-CA", "CA-TAACA"};
    EXPECT_EQ("RMDMMIMM", a2.Transcript());
    EXPECT_FLOAT_EQ(5. / 8, a2.Accuracy());
    EXPECT_EQ(1, a2.Mismatches());
    EXPECT_EQ(1, a2.Deletions());
    EXPECT_EQ(1, a2.Insertions());
    EXPECT_EQ(5, a2.Matches());
}

TEST(Align_PairwiseAlignment, can_generate_basic_global_alignments)
{
    std::unique_ptr<PairwiseAlignment> a{PacBio::Align::Align("GATT", "GATT")};
    EXPECT_FLOAT_EQ(1.0, a->Accuracy());
    EXPECT_EQ("GATT", a->Target());
    EXPECT_EQ("GATT", a->Query());
    EXPECT_EQ("MMMM", a->Transcript());

    a.reset(PacBio::Align::Align("GATT", "GAT"));
    EXPECT_FLOAT_EQ(0.75, a->Accuracy());
    EXPECT_EQ("GATT", a->Target());
    EXPECT_EQ("GA-T", a->Query());
    EXPECT_EQ("MMDM", a->Transcript());

    a.reset(PacBio::Align::Align("GATTACA", "TT"));
    EXPECT_EQ("GATTACA", a->Target());
    EXPECT_EQ("--TT---", a->Query());
    EXPECT_FLOAT_EQ(2. / 7, a->Accuracy());
}

TEST(Align_PairwiseAlignment, maps_target_to_query_positions_from_transcript)
{
    auto areEqual = [](const std::vector<int>& expected, const std::vector<int>& observed) {
        return std::equal(expected.cbegin(), expected.cend(), observed.cbegin());
    };

    {
        SCOPED_TRACE("MMM -> 0123");
        const std::vector<int> expected{0, 1, 2, 3};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MMM")));
    }
    {
        SCOPED_TRACE("DMM -> 0012");
        const std::vector<int> expected{0, 0, 1, 2};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("DMM")));
    }
    {
        SCOPED_TRACE("MDM -> 0112");
        const std::vector<int> expected{0, 1, 1, 2};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MDM")));
    }
    {
        SCOPED_TRACE("MMD -> 0122");
        const std::vector<int> expected{0, 1, 2, 2};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MMD")));
    }
    {
        SCOPED_TRACE("IMM -> 123");
        const std::vector<int> expected{1, 2, 3};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("IMM")));
    }
    {
        SCOPED_TRACE("MIM -> 023");
        const std::vector<int> expected{0, 2, 3};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MIM")));
    }
    {
        SCOPED_TRACE("MMI -> 013");
        const std::vector<int> expected{0, 1, 3};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MMI")));
    }
    {
        SCOPED_TRACE("MRM, MDIM -> 0123");
        const std::vector<int> expected{0, 1, 2, 3};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MRM")));
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MDIM")));
    }
    {
        SCOPED_TRACE("MIDM -> 0223");
        const std::vector<int> expected{0, 2, 2, 3};
        EXPECT_TRUE(areEqual(expected, PacBio::Align::TargetToQueryPositions("MIDM")));
    }
}

// ---------------- Alignment justification tests ----------------------

TEST(Align_PairwiseAlignment, check_justify_internals)
{
    using namespace PacBio::Align::internal;

    std::string t;
    std::string q;
    std::string x;

    // Rewrite2L
    {
        t = "ACCT";
        q = "ACCT";
        x = "MMMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2L(&t, &q, &x, 1));
        EXPECT_EQ("ACCT", t);
        EXPECT_EQ("ACCT", q);
        EXPECT_EQ("MMMM", x);

        t = "ACGT";
        q = "AC-T";
        x = "MMDM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2L(&t, &q, &x, 1));
        EXPECT_EQ("ACGT", t);
        EXPECT_EQ("AC-T", q);
        EXPECT_EQ("MMDM", x);

        t = "ACCT";
        q = "A-CT";
        x = "MDMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2L(&t, &q, &x, 1));
        EXPECT_EQ("ACCT", t);
        EXPECT_EQ("A-CT", q);
        EXPECT_EQ("MDMM", x);

        t = "A-CT";
        q = "ACCT";
        x = "MIMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2L(&t, &q, &x, 1));
        EXPECT_EQ("A-CT", t);
        EXPECT_EQ("ACCT", q);
        EXPECT_EQ("MIMM", x);

        t = "ACCT";
        q = "AC-T";
        x = "MMDM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite2L(&t, &q, &x, 1));
        EXPECT_EQ("ACCT", t);
        EXPECT_EQ("A-CT", q);
        EXPECT_EQ("MDMM", x);

        t = "AC-T";
        q = "ACCT";
        x = "MMIM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite2L(&t, &q, &x, 1));
        EXPECT_EQ("A-CT", t);
        EXPECT_EQ("ACCT", q);
        EXPECT_EQ("MIMM", x);
    }

    // Rewrite3L
    {
        t = "ACGCT";
        q = "ACGCT";
        x = "MMMMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3L(&t, &q, &x, 1));
        EXPECT_EQ("ACGCT", t);
        EXPECT_EQ("ACGCT", q);
        EXPECT_EQ("MMMMM", x);

        t = "ACGGT";
        q = "AC--T";
        x = "MMDDM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3L(&t, &q, &x, 1));
        EXPECT_EQ("ACGGT", t);
        EXPECT_EQ("AC--T", q);
        EXPECT_EQ("MMDDM", x);

        t = "ACGCT";
        q = "A--CT";
        x = "MDDMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3L(&t, &q, &x, 1));
        EXPECT_EQ("ACGCT", t);
        EXPECT_EQ("A--CT", q);
        EXPECT_EQ("MDDMM", x);

        t = "A--CT";
        q = "ACGCT";
        x = "MIIMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3L(&t, &q, &x, 1));
        EXPECT_EQ("A--CT", t);
        EXPECT_EQ("ACGCT", q);
        EXPECT_EQ("MIIMM", x);

        t = "ACGCT";
        q = "AC--T";
        x = "MMDDM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite3L(&t, &q, &x, 1));
        EXPECT_EQ("ACGCT", t);
        EXPECT_EQ("A--CT", q);
        EXPECT_EQ("MDDMM", x);

        t = "AC--T";
        q = "ACGCT";
        x = "MMIIM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite3L(&t, &q, &x, 1));
        EXPECT_EQ("A--CT", t);
        EXPECT_EQ("ACGCT", q);
        EXPECT_EQ("MIIMM", x);
    }

    // Rewrite2R
    {
        t = "ACCT";
        q = "ACCT";
        x = "MMMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2R(&t, &q, &x, 1));
        EXPECT_EQ("ACCT", t);
        EXPECT_EQ("ACCT", q);
        EXPECT_EQ("MMMM", x);

        t = "ACGT";
        q = "AC-T";
        x = "MMDM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2R(&t, &q, &x, 1));
        EXPECT_EQ("ACGT", t);
        EXPECT_EQ("AC-T", q);
        EXPECT_EQ("MMDM", x);

        t = "ACCT";
        q = "AC-T";
        x = "MMDM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2R(&t, &q, &x, 1));
        EXPECT_EQ("ACCT", t);
        EXPECT_EQ("AC-T", q);
        EXPECT_EQ("MMDM", x);

        t = "AC-T";
        q = "ACCT";
        x = "MMIM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite2R(&t, &q, &x, 1));
        EXPECT_EQ("AC-T", t);
        EXPECT_EQ("ACCT", q);
        EXPECT_EQ("MMIM", x);

        t = "ACCT";
        q = "A-CT";
        x = "MDMM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite2R(&t, &q, &x, 1));
        EXPECT_EQ("ACCT", t);
        EXPECT_EQ("AC-T", q);
        EXPECT_EQ("MMDM", x);

        t = "A-CT";
        q = "ACCT";
        x = "MIMM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite2R(&t, &q, &x, 1));
        EXPECT_EQ("AC-T", t);
        EXPECT_EQ("ACCT", q);
        EXPECT_EQ("MMIM", x);
    }

    // Rewrite3R
    {
        t = "ACGCT";
        q = "ACGCT";
        x = "MMMMM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3R(&t, &q, &x, 1));
        EXPECT_EQ("ACGCT", t);
        EXPECT_EQ("ACGCT", q);
        EXPECT_EQ("MMMMM", x);

        t = "ACGGT";
        q = "AC--T";
        x = "MMDDM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3R(&t, &q, &x, 1));
        EXPECT_EQ("ACGGT", t);
        EXPECT_EQ("AC--T", q);
        EXPECT_EQ("MMDDM", x);

        t = "ACGCT";
        q = "AC--T";
        x = "MMDDM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3R(&t, &q, &x, 1));
        EXPECT_EQ("ACGCT", t);
        EXPECT_EQ("AC--T", q);
        EXPECT_EQ("MMDDM", x);

        t = "AC--T";
        q = "ACGCT";
        x = "MMIIM";
        EXPECT_FALSE(PacBio::Align::internal::Rewrite3R(&t, &q, &x, 1));
        EXPECT_EQ("AC--T", t);
        EXPECT_EQ("ACGCT", q);
        EXPECT_EQ("MMIIM", x);

        t = "ACGCT";
        q = "A--CT";
        x = "MDDMM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite3R(&t, &q, &x, 1));
        EXPECT_EQ("ACGCT", t);
        EXPECT_EQ("AC--T", q);
        EXPECT_EQ("MMDDM", x);

        t = "A--CT";
        q = "ACGCT";
        x = "MIIMM";
        EXPECT_TRUE(PacBio::Align::internal::Rewrite3R(&t, &q, &x, 1));
        EXPECT_EQ("AC--T", t);
        EXPECT_EQ("ACGCT", q);
        EXPECT_EQ("MMIIM", x);
    }
}

TEST(Align_PairwiseAlignment, can_left_and_right_justify_alignments)
{
    using LRType = PacBio::Align::LRType;

    // deletion
    {
        PairwiseAlignment a{"AAAAAA", "AAA-AA"};

        a.Justify(LRType::LEFT);
        EXPECT_EQ("AAAAAA", a.Target());
        EXPECT_EQ("-AAAAA", a.Query());
        EXPECT_EQ("DMMMMM", a.Transcript());

        a.Justify(LRType::RIGHT);
        EXPECT_EQ("AAAAAA", a.Target());
        EXPECT_EQ("AAAAA-", a.Query());
        EXPECT_EQ("MMMMMD", a.Transcript());
    }

    // insertion
    {
        PairwiseAlignment a{"A-AAAA", "AAAAAA"};

        a.Justify(LRType::LEFT);
        EXPECT_EQ("-AAAAA", a.Target());
        EXPECT_EQ("AAAAAA", a.Query());
        EXPECT_EQ("IMMMMM", a.Transcript());

        a.Justify(LRType::RIGHT);
        EXPECT_EQ("AAAAA-", a.Target());
        EXPECT_EQ("AAAAAA", a.Query());
        EXPECT_EQ("MMMMMI", a.Transcript());
    }

    // interruption in homopolymer
    {
        PairwiseAlignment a{"GATTTACA", "GAGT-ACA"};

        a.Justify(LRType::LEFT);
        EXPECT_EQ("GATTTACA", a.Target());
        EXPECT_EQ("GAG-TACA", a.Query());
        EXPECT_EQ("MMRDMMMM", a.Transcript());

        a.Justify(LRType::RIGHT);
        EXPECT_EQ("GATTTACA", a.Target());
        EXPECT_EQ("GAGT-ACA", a.Query());
        EXPECT_EQ("MMRMDMMM", a.Transcript());
    }

    // double bases, adjacent
    {
        PairwiseAlignment a{"AAAAAA", "AAA--A"};

        a.Justify(LRType::LEFT);
        EXPECT_EQ("AAAAAA", a.Target());
        EXPECT_EQ("--AAAA", a.Query());
        EXPECT_EQ("DDMMMM", a.Transcript());

        a.Justify(LRType::RIGHT);
        EXPECT_EQ("AAAAAA", a.Target());
        EXPECT_EQ("AAAA--", a.Query());
        EXPECT_EQ("MMMMDD", a.Transcript());
    }

    // double bases, separated
    {
        PairwiseAlignment a{"AAAAAA", "A-AA-A"};

        a.Justify(LRType::LEFT);
        EXPECT_EQ("AAAAAA", a.Target());
        EXPECT_EQ("--AAAA", a.Query());
        EXPECT_EQ("DDMMMM", a.Transcript());

        a.Justify(LRType::RIGHT);
        EXPECT_EQ("AAAAAA", a.Target());
        EXPECT_EQ("AAAA--", a.Query());
        EXPECT_EQ("MMMMDD", a.Transcript());
    }

    // intervening insertion
    {
        PairwiseAlignment a{"A----A", "AATAAA"};

        a.Justify(LRType::LEFT);
        EXPECT_EQ("----AA", a.Target());
        EXPECT_EQ("AATAAA", a.Query());
        EXPECT_EQ("IIIIMM", a.Transcript());

        a.Justify(LRType::RIGHT);
        EXPECT_EQ("AA----", a.Target());
        EXPECT_EQ("AATAAA", a.Query());
        EXPECT_EQ("MMIIII", a.Transcript());
    }

    // intervening match
    {
        PairwiseAlignment a{"A-T--A", "AATAAA"};

        a.Justify(LRType::LEFT);
        EXPECT_EQ("-AT--A", a.Target());
        EXPECT_EQ("AATAAA", a.Query());
        EXPECT_EQ("IMMIIM", a.Transcript());

        a.Justify(LRType::RIGHT);
        EXPECT_EQ("A-TA--", a.Target());
        EXPECT_EQ("AATAAA", a.Query());
        EXPECT_EQ("MIMMII", a.Transcript());
    }
}

// ------------------ AffineAlignment tests ---------------------

TEST(Align_AffineAlignment, can_generate_basic_affine_alignments)
{
    std::unique_ptr<PairwiseAlignment> a{PacBio::Align::AlignAffine("ATT", "ATT")};
    EXPECT_EQ("ATT", a->Target());
    EXPECT_EQ("ATT", a->Query());

    a.reset(PacBio::Align::AlignAffine("AT", "ATT"));
    EXPECT_EQ("A-T", a->Target());
    EXPECT_EQ("ATT", a->Query());

    a.reset(PacBio::Align::AlignAffine("GA", "GAT"));
    EXPECT_EQ("GA-", a->Target());
    EXPECT_EQ("GAT", a->Query());

    a.reset(PacBio::Align::AlignAffine("GAT", "GA"));
    EXPECT_EQ("GAT", a->Target());
    EXPECT_EQ("GA-", a->Query());

    a.reset(PacBio::Align::AlignAffine("GA", "TGA"));
    EXPECT_EQ("-GA", a->Target());
    EXPECT_EQ("TGA", a->Query());

    a.reset(PacBio::Align::AlignAffine("TGA", "GA"));
    EXPECT_EQ("TGA", a->Target());
    EXPECT_EQ("-GA", a->Query());

    a.reset(PacBio::Align::AlignAffine("GATTACA", "GATTTACA"));
    EXPECT_EQ("GA-TTACA", a->Target());
    EXPECT_EQ("GATTTACA", a->Query());
}

TEST(Align_AffineAlignment, correctly_aligns_large_insertion)
{
    // Test a real-world large insertion, found in an E. Coli
    // experiment
    const char* target =
        "AACGATTTTATGATGGCATGTGACATGTATTTCCGTTGGGGGCATTTTAATAAGTGAGGA"
        "AGTGATAGGAAGTGACCAGATAATACATATATGTTCTGTACTCTCTTGCGCATTTTGATT"
        "GTTGACTGAGTAACCAGACAGTTGATGTGCACGATTTCCCCTCGCCCTAACAGACGTGGG"
        "CGGGGGCACCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCTCTT"
        "CTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCCGC"
        "TCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGACCCCCGGTCGGGGCT"
        "TCTCATCCCCCCGGTGTGTGCAATACACGAAAAAAAAGCCCGTACTTTCGTACGAGCTCT"
        "TCTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCCG"
        "CTCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGAACCCCGGTCGGGGC"
        "TTCTCATCCCCCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCTC"
        "TTCTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCC"
        "GCTCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGAACCCCGGTCGGGG"
        "CTTCTCATCCCCCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCT"
        "CTTCTTTAAATATGGCGGTGAGGGGGGGATTCGAACCCCCGATACGTTGCCGTATACACA"
        "CTTTCCAGGCGTGCTCCTTCAGCCACTCGGACACCTCACCAAATTGTCGTTCCTGTCTTG"
        "CTGGAACGGGCGCTAATTTAGGGAAATCATGACCTGAGGTCAACAAACTTTTTGAAAAAA"
        "TCGCGCGTTTATTCAAACTTCAATCAATGTGTGGTTTTAATAAGCGAAAT";

    const char* query =
        "AACGATTTTATGATGGCATGTGACATGTATTTCCGTTGGGGGCATTTTAATAAGTGAGGA"
        "AGTGATAGGAAGTGACCAGATAATACATATATGTTCTGTACTCTCTTGCGCATTTTGATT"
        "GTTGACTGAGTAACCAGACAGTTGATGTGCACGATTTCCCCTCGCCCTAACAGACGTGGG"
        "CGGGGGCACCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCTCTT"
        "CTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCCGC"
        "TCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGACCCCCGGTCGGGGCT"
        "TCTCATCCCCCCGGTGTGTGCAATACACGAAAAAAAAGCCCGTACTTTCGTACGAGCTCT"
        "TCTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCCG"
        "CTCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGAACCCCGGTCGGGGC"
        "TTCTCATCCCCCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCTC"
        "TTCTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCC"
        "GCTCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGAACCCCGGTCGGGG"
        "CTTCTCATCCCCCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCT"
        "CTTCTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCC"
        "CGCTCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGAACCCCGGTCGGG"
        "GCTTCTCATCCCCCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGC"
        "TCTTCTTTAAATATGGCGGTGAGGGGGGGATTCGAACCCCCGATACGTTGCCGTATACAC"
        "ACTTTCCAGGCGTGCTCCTTCAGCCACTCGGACACCTCACCAAATTGTCGTTCCTGTCTT"
        "GCTGGAACGGGCGCTAATTTAGGGAAATCATGACCTGAGGTCAACAAACTTTTTGAAAAA"
        "ATCGCGCGTTTATTCAAACTTCAATCAATGTGTGGTTTTAATAAGCGAAAT";

    const char* expectedAlignedTarget =
        "AACGATTTTATGATGGCATGTGACATGTATTTCCGTTGGGGGCATTTTAATAAGTGAGGA"
        "AGTGATAGGAAGTGACCAGATAATACATATATGTTCTGTACTCTCTTGCGCATTTTGATT"
        "GTTGACTGAGTAACCAGACAGTTGATGTGCACGATTTCCCCTCGCCCTAACAGACGTGGG"
        "CGGGGGCACCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCTCTT"
        "CTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCCGC"
        "TCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGACCCCCGGTCGGGGCT"
        "TCTCATCCCCCCGGTGTGTGCAATAC----------------------------------"
        "------------------------------------------------------------"
        "------------------------------------------------------------"
        "---------------------------ACGAAAAAAAAGCCCGTACTTTCGTACGAGCTC"
        "TTCTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCCC"
        "GCTCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGAACCCCGGTCGGGG"
        "CTTCTCATCCCCCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGCT"
        "CTTCTTTAAATATGGCGGTGAGGGGGGGATTGACTCGCTTCGCTCGCCCTGCGGGCAGCC"
        "CGCTCACTGCGTTCACGGTCTGTCCAACTGGCTGTCGCCAGTTGTCGAACCCCGGTCGGG"
        "GCTTCTCATCCCCCCGGTGTGTGCAATATACGAAAAAAAAGCCCGTACTTTCGTACGAGC"
        "TCTTCTTTAAATATGGCGGTGAGGGGGGGATTCGAACCCCCGATACGTTGCCGTATACAC"
        "ACTTTCCAGGCGTGCTCCTTCAGCCACTCGGACACCTCACCAAATTGTCGTTCCTGTCTT"
        "GCTGGAACGGGCGCTAATTTAGGGAAATCATGACCTGAGGTCAACAAACTTTTTGAAAAA"
        "ATCGCGCGTTTATTCAAACTTCAATCAATGTGTGGTTTTAATAAGCGAAAT";

    std::unique_ptr<PairwiseAlignment> a{PacBio::Align::AlignAffine(target, query)};
    ASSERT_EQ(expectedAlignedTarget, a->Target());
}

// ------------------ IUPAC-aware alignment tests ---------------------

TEST(Align_AffineAlignment, can_generate_IUPAC_aware_alignments)
{
    std::unique_ptr<PairwiseAlignment> a{PacBio::Align::AlignAffineIupac("GATTTT", "GMTTT")};
    ASSERT_EQ("GATTTT", a->Target());
    ASSERT_EQ("GM-TTT", a->Query());

    a.reset(PacBio::Align::AlignAffineIupac("TTTTAG", "TTTMG"));
    ASSERT_EQ("TTTTAG", a->Target());
    ASSERT_EQ("-TTTMG", a->Query());
}

// ---------------- Linear-space alignment tests -----------------------

TEST(Align_LinearAlignment, can_generate_basic_linear_alignments)
{
    const PacBio::Align::AlignParams params{2, -1, -2, -2};
    const PacBio::Align::AlignConfig config{params, PacBio::Align::AlignMode::GLOBAL};

    int score = 0;
    std::unique_ptr<PairwiseAlignment> a{PacBio::Align::AlignLinear("GATTACA", "GATTACA", &score)};
    EXPECT_EQ("GATTACA", a->Target());
    EXPECT_EQ("GATTACA", a->Query());
    EXPECT_EQ("MMMMMMM", a->Transcript());
    EXPECT_EQ(14, score);

    a.reset(PacBio::Align::AlignLinear("CGAC", "GAAAACGAC", &score));
    EXPECT_EQ("-----CGAC", a->Target());
    EXPECT_EQ("GAAAACGAC", a->Query());
    EXPECT_EQ("IIIIIMMMM", a->Transcript());

    a.reset(PacBio::Align::AlignLinear(
        "CATCAGGTAAGAAAGTACGATGCTACAGCTTGTGACTGGTGCGGCACTTTTGGCTGAGTTTCCTGTCCACCTCATGTATTCTGCCCTAAC"
        "GTCGGTCTTCACGCCATTACTAGACCGACAAAATGGAACCGGGGCCTTAAACCCCGTTCGAGGCGTAGCAAGGAGATAGGGTTATGAACT"
        "CTCCCAGTCAATATACCAACACATCGTGGGACGGATTGCAGAGCGAATCTATCCGCGCTCGCATAATTTAGTGTTGATC",
        "CATCAGGTAAGAAAAGTACGATGCTACAGCTGTGACTGGTGCGGCACTTTTTGGCCTGAGTTTCCTGTCCACTCATGTATTCTGGCCCTA"
        "ACTTAGGTCGGTCTTCACGCCATTTACTAGCACGAAAACGACAAAATTGGAAGCCGGGGCCTAAACACCCGTTCGAGGCGGTAGCAAGGA"
        "GATTAGGGTTATGAACTCTCCCAGTCAATGATACAAACAATCGTGGGACGGAATTGCAGAGCGAATCTATCCGCGCTCAAGCATAATTTA"
        "GTGTTGATC",
        &score));

    a.reset(PacBio::Align::AlignLinear(
        "CATCAGGTAAGAAAGTACGATGCTACAGCTTGTGACTGGTGCGGCACTTTTGGCTGAGTTTCCTGTCCACCTCATGTATTCTGCCCTAAC"
        "GTCGGTCTTCACGCCATTACTAGACCGACAAAATGGAAGCCGGGGCCTTAAACCCCGTTCGAGGCGTAGCAAGGAGATAGGGTTATGAAC"
        "TCTCCCAGTCAATATACCAACACATCGTGGGACGGATTGCAGAGCGAATCTATCCGCGCTCGCATAATTTAGTGTTGATC",
        "CCCCGGGAATCTCTAGAATGCATCAGGTAAGAAAGTAACGATGCTTACACTTGTGACTGGTTGCGGCACTTTTGGTGAGTTTCCTGTCCA"
        "CTCAATGTATTCTGCCTAACGTCGTGTCTTCACGCCATTTACTAGACCGAGAAGGAAATTGGAAGGCCCGGGGGCCTTAAACGCCCGTTC"
        "GAGCGTAGCTAAGGAGATAGGGTTATGAACTCTCCCAGTCATATAGCCAACACATCGTGGAACGGAATTGCAGAGCGAATCTATCCGCTG"
        "CTCGCATAAATTTAGTGTTGATCCATAAAGCTTGCTGAGGACTAGTAGCTT",
        &score));

    a.reset(PacBio::Align::AlignLinear("TATGC", "AGTACGCA", &score));
    EXPECT_EQ("--TATGC-", a->Target());
    EXPECT_EQ("AGTACGCA", a->Query());
    EXPECT_EQ("IIMMRMMI", a->Transcript());
    EXPECT_EQ(1, score);

    a.reset(PacBio::Align::AlignLinear("AGTACGCA", "TATGC", &score));
    EXPECT_EQ("AGTACGCA", a->Target());
    EXPECT_EQ("--TATGC-", a->Query());
    EXPECT_EQ("DDMMRMMD", a->Transcript());
    EXPECT_EQ(1, score);

    a.reset(PacBio::Align::AlignLinear("GATT", "GATT"));
    EXPECT_FLOAT_EQ(1.0, a->Accuracy());
    EXPECT_EQ("GATT", a->Target());
    EXPECT_EQ("GATT", a->Query());
    EXPECT_EQ("MMMM", a->Transcript());

    a.reset(PacBio::Align::AlignLinear("GATT", "GAT"));
    EXPECT_FLOAT_EQ(0.75, a->Accuracy());
    EXPECT_EQ("GATT", a->Target());
    EXPECT_EQ("GA-T", a->Query());
    EXPECT_EQ("MMDM", a->Transcript());

    a.reset(PacBio::Align::AlignLinear("GATTACA", "TT"));
    EXPECT_EQ("GATTACA", a->Target());
    EXPECT_EQ("--TT---", a->Query());
    EXPECT_FLOAT_EQ(2. / 7, a->Accuracy());

    const char* ref =
        "GTATTTTAAATAAAAACATTAAGTTATGACGAAGAAGAACGGAAACGCCTTAAACCGGAAAATTTTCATAAATAGCGAAAACCCGCGAGG"
        "TCGCCGCCC";
    const char* read =
        "GTATTTTAAATAAAAAAACATTATAGTTTAATGAACGAGAATGAACGGTAATACGCCTTTAAAGCCTGAAATATTTTTCCATAAATGTAA"
        "TTTCTGTATATAATCTCCGCGAGTGTCTGCCGCCC";

    a.reset(PacBio::Align::AlignLinear(ref, read, &score));
    int peerScore = 0;
    std::unique_ptr<PairwiseAlignment> peerAlignment{
        PacBio::Align::Align(ref, read, &peerScore, config)};
    EXPECT_EQ(score, peerScore);
}

#if 0
TEST(LinearAlignmentTests, SemiglobalTests)
{
    AlignParams params(2, -1, -2, -2);
    int score, peerScore;
    std::unique_ptr<PairwiseAlignment> a{AlignLinear("AGTCGATACACCCC", "GATTACA")};

    EXPECT_EQ("AGTCGA-TACACCCC", a->Target());
    EXPECT_EQ("----GATTACA----", a->Query());

    // we got:
    // -AGTCGATACACCCC
    // GA-T---TACA----

}
#endif

// ------------------ Local alignment tests ---------------------

TEST(Align_LocalAlignment, can_generate_basic_local_alignments)
{
    const std::string target{"CAGCCTTTCTGACCCGGAAATCAAAATAGGCACAACAAA"};
    const std::string query{"CTGAGCCGGTAAATC"};

    const auto a = PacBio::Align::LocalAlign(target, query);
    EXPECT_EQ(8, a.TargetBegin());
    EXPECT_EQ(21, a.TargetEnd());
    EXPECT_EQ(0, a.QueryBegin());
    EXPECT_EQ(14, a.QueryEnd());
    EXPECT_EQ(2, a.NumMismatches());
    EXPECT_EQ(21, a.Score());
}

// --------------- Semi-Global alignment tests ------------------

TEST(Align_SemiGlobalAlignment, can_generate_basic_semiglobal_alignments)
{
    const std::string target{"CAGCCTTTCTGACCCGGAAATCAAAATAGGCACAACAAA"};
    const std::string query{"CTGAGCCGGTAAATC"};
    const PacBio::Align::AlignConfig cfg{PacBio::Align::AlignParams::Default(),
                                         PacBio::Align::AlignMode::SEMIGLOBAL};

    std::unique_ptr<PairwiseAlignment> pa{PacBio::Align::Align(target, query, cfg)};
    EXPECT_EQ(13, pa->Matches());
    EXPECT_EQ(2, pa->Errors());
    EXPECT_EQ(7, pa->ReferenceStart());
    EXPECT_EQ(21, pa->ReferenceEnd());
}
