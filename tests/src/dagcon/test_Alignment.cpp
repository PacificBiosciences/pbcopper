
#include <gtest/gtest.h>

#include <pbcopper/dagcon/Alignment.h>

using namespace PacBio;

// clang-format off

TEST(Dagcon_Alignment, can_normalize_gaps_on_empty_strings)
{
    const Dagcon::Alignment aln;
    const auto norm = Dagcon::NormalizeGaps(aln);
    EXPECT_TRUE(norm.Query.empty());
    EXPECT_TRUE(norm.Target.empty());
}

TEST(Dagcon_Alignment, can_normalize_gaps_on_simple_mismatch)
{
    Dagcon::Alignment aln;
    aln.Query  = "CAC";
    aln.Target = "CGC";

    const auto norm = Dagcon::NormalizeGaps(aln);
    EXPECT_EQ("C-AC", norm.Query);
    EXPECT_EQ("CG-C", norm.Target);
}

TEST(Dagcon_Alignment, can_normalize_gaps_shifting_query)
{
    Dagcon::Alignment aln;
    aln.Query  = "-C--CGT";
    aln.Target = "CCGAC-T";

    const auto norm = Dagcon::NormalizeGaps(aln);
    EXPECT_EQ("CCG--T", norm.Query);
    EXPECT_EQ("CCGACT", norm.Target);
}

TEST(Dagcon_Alignment, can_normalize_gaps_shifting_target)
{
    Dagcon::Alignment aln;
    aln.Query  = "ATAT-AGCCGGC";
    aln.Target = "ATATTA---GGC";

    const auto norm = Dagcon::NormalizeGaps(aln);
    EXPECT_EQ("ATAT-AGCCGGC", norm.Query);
    EXPECT_EQ("ATATTAG--G-C", norm.Target);
}

TEST(Dagcon_Alignment, negative_or_zero_trim_yields_original_alignment)
{
    const std::string q{"AC-C-C-T---"};
    const std::string t{"ACG-TCA-GCA"};

    {   // trim length = 0
        Dagcon::Alignment aln;
        aln.Query  = q;
        aln.Target = t;
        aln.Start  = 1;
        aln.Strand = '-';

        Dagcon::TrimAlignment(aln, 0);
        EXPECT_EQ(q, aln.Query);
        EXPECT_EQ(t, aln.Target);
        EXPECT_EQ(1, aln.Start);
        EXPECT_EQ('-', aln.Strand);
    }

    {   // trim length = -5
        Dagcon::Alignment aln;
        aln.Query  = q;
        aln.Target = t;
        aln.Start  = 1;
        aln.Strand = '-';

        Dagcon::TrimAlignment(aln, -5);
        EXPECT_EQ(q, aln.Query);
        EXPECT_EQ(t, aln.Target);
        EXPECT_EQ(1, aln.Start);
        EXPECT_EQ('-', aln.Strand);
    }
}

TEST(Dagcon_Alignment, extra_large_trim_yields_empty_alignment)
{
    const std::string q{"AC-C-C-T---"};
    const std::string t{"ACG-TCA-GCA"};

    Dagcon::Alignment aln;
    aln.Query  = q;
    aln.Target = t;
    aln.Start  = 1;
    aln.Strand = '-';

    Dagcon::TrimAlignment(aln, 500);
    EXPECT_EQ("", aln.Query);
    EXPECT_EQ("", aln.Target);
    EXPECT_EQ('-', aln.Strand);

    // NOTE (DB): this line pulled verbatim from pbdagcon/dagger
    //
    // EXPECT_EQ(1 + 9, aln.start); // start could be anything, really
}

TEST(Dagcon_Alignment, can_trim_normal_alignment)
{
    auto makeAlignment = []()
    {
        //                   123 456 789
        const std::string t{"ACG-TCA-GCA"};
        const std::string q{"AC-C-C-T---"};

        Dagcon::Alignment aln;
        aln.Target = t;
        aln.Query  = q;
        aln.Start  = 1;
        aln.Strand = '-';
        return aln;
    };

    {   // trim length = 3
        auto aln = makeAlignment();
        Dagcon::TrimAlignment(aln, 3);
        EXPECT_EQ(4, aln.Start);
        EXPECT_EQ("-TCA-", aln.Target);
        EXPECT_EQ("C-C-T", aln.Query);
    }

    {   // trim length = 4
        auto aln = makeAlignment();
        Dagcon::TrimAlignment(aln, 4);
        EXPECT_EQ(5, aln.Start);
        EXPECT_EQ("C", aln.Target);
        EXPECT_EQ("C", aln.Query);
    }

    {   // trim length = 5
        auto aln = makeAlignment();
        Dagcon::TrimAlignment(aln, 5);
        EXPECT_EQ(6, aln.Start);
        EXPECT_EQ("", aln.Target);
        EXPECT_EQ("", aln.Query);
    }
}

// clang-format on
