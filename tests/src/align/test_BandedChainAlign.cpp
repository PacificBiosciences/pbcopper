// Author: Derek Barnett

#include <gtest/gtest.h>

#include <pbcopper/align/BandedChainAlignment.h>
#include <pbcopper/align/internal/BCAlignBlocks.h>
#include <pbcopper/align/internal/BCAlignImpl.h>

TEST(Align_BandedChainAlignment, can_generate_alignments_in_standard_block)
{
    using Config = PacBio::Align::BandedChainAlignConfig;
    using Alignment = PacBio::Align::BandedChainAlignment;
    using Block = PacBio::Align::Internal::StandardGlobalAlignBlock;

    const Config config = Config::Default();
    Block block{config};

    {  // complete sequence match
        const char* t = "ATT";
        const char* q = "ATT";
        const size_t tLen = 3;
        const size_t qLen = 3;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("MMM", cigar);
        EXPECT_EQ(6, align.Score());
    }
    {  // ensure gap at end (query)
        const char* t = "ATT";
        const char* q = "AT";
        const size_t tLen = 3;
        const size_t qLen = 2;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("MMD", cigar);
        EXPECT_EQ(4, align.Score());  // end-gaps free
    }
    {  // ensure gap at end (target)
        const char* t = "AT";
        const char* q = "ATT";
        const size_t tLen = 2;
        const size_t qLen = 3;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("MMI", cigar);
        EXPECT_EQ(4, align.Score());  // end-gaps free
    }
    {  // query end insertion
        const char* t = "GA";
        const char* q = "GAT";
        const size_t tLen = 2;
        const size_t qLen = 3;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("MMI", cigar);
        EXPECT_EQ(4, align.Score());  // end-gaps free
    }
    {  // query end deletion
        const char* t = "GAT";
        const char* q = "GA";
        const size_t tLen = 3;
        const size_t qLen = 2;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("MMD", cigar);
        EXPECT_EQ(4, align.Score());  // end-gaps free
    }
    {  // query begin insertion
        const char* t = "GA";
        const char* q = "TGA";
        const size_t tLen = 2;
        const size_t qLen = 3;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("IMM", cigar);
        EXPECT_EQ(4, align.Score());  // end-gaps free
    }
    {  // query begin deletion
        const char* t = "TGA";
        const char* q = "GA";
        const size_t tLen = 3;
        const size_t qLen = 2;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("DMM", cigar);
        EXPECT_EQ(4, align.Score());  // end-gaps free
    }
    {  // (slightly longer) internal insertion
        const char* t = "GATTACA";
        const char* q = "GATTTACA";
        const size_t tLen = 7;
        const size_t qLen = 8;
        const auto cigar = block.Align(t, tLen, q, qLen);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};
        EXPECT_EQ("MMIMMMMM", cigar);
        EXPECT_EQ(12, align.Score());
    }
}

TEST(Align_BandedChainAlignment, standard_block_correctly_aligns_large_insertion)
{
    const std::string target =
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

    const std::string query =
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

    // "386=181I624="
    std::string expectedAlignment(386, 'M');
    expectedAlignment += std::string(181, 'I');
    expectedAlignment += std::string(624, 'M');

    using Config = PacBio::Align::BandedChainAlignConfig;
    using Block = PacBio::Align::Internal::StandardGlobalAlignBlock;
    const Config config{2, -1, -2, -1, 1};
    Block block{config};
    const auto cigar = block.Align(target.c_str(), target.size(), query.c_str(), query.size());
    EXPECT_EQ(expectedAlignment, cigar);
}

TEST(Align_BandedChainAlignment, can_generate_alignments_in_banded_block)
{
    using Config = PacBio::Align::BandedChainAlignConfig;
    using Alignment = PacBio::Align::BandedChainAlignment;
    using Block = PacBio::Align::Internal::BandedGlobalAlignBlock;
    using Seed = PacBio::Align::Seed;

    Config config = Config::Default();
    config.bandExtend_ = 2;
    Block block{config};

    {
        const char* t = "GATTACAT";
        const char* q = "GATTACAT";
        const size_t tLen = 8;
        const size_t qLen = 8;
        const auto seed = PacBio::Align::Seed{0, 0, 8};

        const auto cigar = block.Align(t, q, seed);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};

        EXPECT_EQ("MMMMMMMM", cigar);
        EXPECT_EQ(16, align.Score());
    }
    {
        const char* t = "ATAGAT";
        const char* q = "ATGT";
        const size_t tLen = 6;
        const size_t qLen = 4;
        const Seed seed{0, 0, 6, 4};

        const auto cigar = block.Align(t, q, seed);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};

        // ATAGAT
        // AT-G-T
        EXPECT_EQ("MMDMDM", cigar);
        EXPECT_EQ(4, align.Score());
    }
    {
        const char* t = "AAAAAATTTTTGGG";
        const char* q = "TTTTTTTTGGGGGGGG";
        const size_t tLen = 14;
        const size_t qLen = 16;
        const Seed seed{0, 0, 14, 16};  // no offset

        const auto cigar = block.Align(t, q, seed);
        const auto align = Alignment{config, t, tLen, q, qLen, cigar};

        // Expected:
        // 2D4X4=1X3=4I
        // AAAAAATTTTTGGG----
        // --TTTTTTTTGGGGGGGG

        EXPECT_EQ("DDRRRRMMMMRMMMIIII", cigar);
        EXPECT_EQ(9, align.Score());  // end-gaps free
    }
}

TEST(Align_BandedChainAlignment, can_stitch_cigars)
{
    using Config = PacBio::Align::BandedChainAlignConfig;
    using Aligner = PacBio::Align::Internal::BandedChainAlignerImpl;

    const Config config = Config::Default();

    {  // simple
        std::string global("MMMMM");
        std::string local("MM");

        Aligner a{config};
        a.StitchTranscripts(&global, std::move(local));

        EXPECT_EQ("MMMMMMM", global);
    }
    {  // different at edge
        std::string global("MMMMMDDD");
        std::string local("MMR");

        Aligner a{config};
        a.StitchTranscripts(&global, std::move(local));

        EXPECT_EQ("MMMMMDDDMMR", global);
    }
}

TEST(Align_BandedChainAlignment, can_generate_alignment_from_input_seeds)
{
    using Config = PacBio::Align::BandedChainAlignConfig;
    using Seed = PacBio::Align::Seed;

    {
        Config config = Config::Default();
        config.bandExtend_ = 2;
        const std::string target = "CGAATCCATCCCACACA";
        const std::string query = "GGCGATNNNCATGGCACA";
        const auto seeds =
            std::vector<Seed>{Seed{0, 2, 5, 6}, Seed{6, 9, 9, 12}, Seed{11, 14, 17, 16}};

        const auto result = PacBio::Align::BandedChainAlign(target, query, seeds, config);

        EXPECT_EQ("--CGAATC--CATCCCACACA", result.alignedTarget_);
        EXPECT_EQ("GGCG-ATNNNCATGGCACA--", result.alignedQuery_);
        EXPECT_EQ("IIMMDMMRIIMMMRRMMMMDD", result.transcript_);
        EXPECT_EQ(14, result.Score());  // end-gaps free
    }
}
