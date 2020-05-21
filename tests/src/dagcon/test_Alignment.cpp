
#include <gtest/gtest.h>

#include <pbcopper/dagcon/Alignment.h>

using namespace PacBio;

// clang-format off

TEST(Dagcon_Alignment, can_normalize_gaps_on_empty_strings)
{
    Dagcon2::Alignment aln;
    Dagcon2::NormalizeGaps(aln);
    EXPECT_TRUE(aln.Query.empty());
    EXPECT_TRUE(aln.Target.empty());
}

TEST(Dagcon_Alignment, can_normalize_gaps_on_simple_mismatch)
{
    Dagcon2::Alignment aln;
    aln.Query  = "CAC";
    aln.Target = "CGC";

    Dagcon2::NormalizeGaps(aln);
    EXPECT_EQ("C-AC", aln.Query);
    EXPECT_EQ("CG-C", aln.Target);
}

TEST(Dagcon_Alignment, can_normalize_gaps_shifting_query)
{
    Dagcon2::Alignment aln;
    aln.Query  = "-C--CGT";
    aln.Target = "CCGAC-T";

    Dagcon2::NormalizeGaps(aln);
    EXPECT_EQ("CCG--T", aln.Query);
    EXPECT_EQ("CCGACT", aln.Target);
}

TEST(Dagcon_Alignment, can_normalize_gaps_shifting_target)
{
    Dagcon2::Alignment aln;
    aln.Query  = "ATAT-AGCCGGC";
    aln.Target = "ATATTA---GGC";

    Dagcon2::NormalizeGaps(aln);
    EXPECT_EQ("ATAT-AGCCGGC", aln.Query);
    EXPECT_EQ("ATATTAG--G-C", aln.Target);
}

TEST(Dagcon_Alignment, negative_or_zero_trim_yields_original_alignment)
{
    const std::string q{"AC-C-C-T---"};
    const std::string t{"ACG-TCA-GCA"};

    {   // trim length = 0
        Dagcon2::Alignment aln;
        aln.Query  = q;
        aln.Target = t;
        aln.Start  = 1;
        aln.Strand = '-';

        Dagcon2::TrimAlignment(aln, 0);
        EXPECT_EQ(q, aln.Query);
        EXPECT_EQ(t, aln.Target);
        EXPECT_EQ(1, aln.Start);
        EXPECT_EQ('-', aln.Strand);
    }

    {   // trim length = -5
        Dagcon2::Alignment aln;
        aln.Query  = q;
        aln.Target = t;
        aln.Start  = 1;
        aln.Strand = '-';

        Dagcon2::TrimAlignment(aln, -5);
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

    Dagcon2::Alignment aln;
    aln.Query  = q;
    aln.Target = t;
    aln.Start  = 1;
    aln.Strand = '-';

    Dagcon2::TrimAlignment(aln, 500);
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

        Dagcon2::Alignment aln;
        aln.Target = t;
        aln.Query  = q;
        aln.Start  = 1;
        aln.Strand = '-';
        return aln;
    };

    {   // trim length = 3
        auto aln = makeAlignment();
        Dagcon2::TrimAlignment(aln, 3);
        EXPECT_EQ(4, aln.Start);
        EXPECT_EQ("-TCA-", aln.Target);
        EXPECT_EQ("C-C-T", aln.Query);
    }

    {   // trim length = 4
        auto aln = makeAlignment();
        Dagcon2::TrimAlignment(aln, 4);
        EXPECT_EQ(5, aln.Start);
        EXPECT_EQ("C", aln.Target);
        EXPECT_EQ("C", aln.Query);
    }

    {   // trim length = 5
        auto aln = makeAlignment();
        Dagcon2::TrimAlignment(aln, 5);
        EXPECT_EQ(6, aln.Start);
        EXPECT_EQ("", aln.Target);
        EXPECT_EQ("", aln.Query);
    }
}

// clang-format on
