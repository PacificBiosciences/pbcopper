#include <pbcopper/algorithm/Heteroduplex.h>
#include <pbcopper/algorithm/internal/HeteroduplexUtils.h>

#include <gtest/gtest.h>

using namespace PacBio;

// clang-format off

TEST(Algorithm_Heteroduplex, can_count_mismatched_bases_at_pos)
{
    // "normal"
    const Algorithm::internal::BaseCount counts{1, 2, 3, 4, 5};
    EXPECT_EQ(6, Algorithm::internal::MismatchCount(counts, 'T'));

    // deletions only
    const Algorithm::internal::BaseCount delOnlyCounts{0, 0, 3, 0, 5};
    EXPECT_EQ(0, Algorithm::internal::MismatchCount(delOnlyCounts, 'G'));

    // reference only
    const Algorithm::internal::BaseCount refOnlyCounts{0, 7, 0, 0, 0};
    EXPECT_EQ(0, Algorithm::internal::MismatchCount(refOnlyCounts, 'C'));
}

TEST(Algorithm_Heteroduplex, empty_cigar_counts_from_empty_input)
{
    const std::string reference;
    const std::vector<std::string> seqs;
    const std::vector<PacBio::Data::Cigar> cigars;
    const std::vector<int32_t> positions;

    const auto cigarCounts =
        Algorithm::internal::CigarMismatchCounts(reference, seqs, cigars, positions);
    EXPECT_TRUE(cigarCounts.MostCommonBases.empty());
}

TEST(Algorithm_Heteroduplex, perfect_match_cigar_counts)
{
    const std::string reference{
        "GATTACAGATTACA"
    };
    const std::vector<std::string> seqs{
        "GATTACAGATTACA",
         "ATTACAGATTAC",
        "GATTACAGATTA",
          "TTACAGATTACA",
    };
    const std::vector<PacBio::Data::Cigar> cigars{
        PacBio::Data::Cigar{"14="},
        PacBio::Data::Cigar{"12="},
        PacBio::Data::Cigar{"12="},
        PacBio::Data::Cigar{"12="},
    };
    const std::vector<int32_t> positions{0, 1, 0, 2};

    const std::vector<int> expectedNumReads{
        2,3,4,4,4,4,4,4,4,4,4,4,3,2
    };
    const std::string expectedMostCommonBases{"GATTACAGATTACA"};
    const std::vector<Algorithm::internal::BaseCount> expectedBaseCounts {
        // array index - A:0, C:1, G:2, T:3, -:4
        {0,0,2,0,0}, {3,0,0,0,0}, {0,0,0,4,0}, {0,0,0,4,0}, {4,0,0,0,0}, {0,4,0,0,0}, {4,0,0,0,0},
        {0,0,4,0,0}, {4,0,0,0,0}, {0,0,0,4,0}, {0,0,0,4,0}, {4,0,0,0,0}, {0,3,0,0,0}, {2,0,0,0,0},
    };

    const auto cigarCounts =
        Algorithm::internal::CigarMismatchCounts(reference, seqs, cigars, positions);

    EXPECT_EQ(expectedNumReads, cigarCounts.NumReads);
    EXPECT_EQ(expectedMostCommonBases, cigarCounts.MostCommonBases);
    EXPECT_EQ(expectedBaseCounts, cigarCounts.BaseCounts);
}

TEST(Algorithm_Heteroduplex, cigar_counts_with_mismatches_and_indels)
{
    const std::string reference{
        "GATTACAGATTACA"
    };
    const std::vector<std::string> seqs{
        "GAATTACAAATTACA",
         "ATAACAGATTAC",
        "GATTACAGATA",
          "TTAAGATTACA",
    };
    const std::vector<PacBio::Data::Cigar> cigars{
        PacBio::Data::Cigar{"2=1I5=1X6="},
        PacBio::Data::Cigar{"2=1X9="},
        PacBio::Data::Cigar{"9=1D2="},
        PacBio::Data::Cigar{"3=1D8="},
    };
    const std::vector<int32_t> positions{0, 1, 0, 2};

    const std::vector<int> expectedNumReads{
        2,3,4,4,4,4,4,4,4,4,4,4,3,2
    };
    const std::string expectedMostCommonBases{"GATTACAGATTACA"};
    const std::vector<Algorithm::internal::BaseCount> expectedBaseCounts{
        // array index - A:0, C:1, G:2, T:3, -:4
        {0,0,2,0,0}, {3,0,0,0,0}, {0,0,0,4,0}, {1,0,0,3,0}, {4,0,0,0,0}, {0,3,0,0,1}, {4,0,0,0,0},
        {1,0,3,0,0}, {4,0,0,0,0}, {0,0,0,3,1}, {0,0,0,4,0}, {4,0,0,0,0}, {0,3,0,0,0}, {2,0,0,0,0},
    };

    const auto cigarCounts =
        Algorithm::internal::CigarMismatchCounts(reference, seqs, cigars, positions);

    EXPECT_EQ(expectedNumReads, cigarCounts.NumReads);
    EXPECT_EQ(expectedMostCommonBases, cigarCounts.MostCommonBases);
    EXPECT_EQ(expectedBaseCounts, cigarCounts.BaseCounts);
}

TEST(Algorithm_Heteroduplex, empty_reference_yields_no_call)
{
    const std::string reference;
    const std::vector<std::string> fwdSeqs{"ACGT"};
    const std::vector<std::string> revSeqs{"ACGT"};
    const std::vector<PacBio::Data::Cigar> fwdCigars{PacBio::Data::Cigar{"4="}};
    const std::vector<PacBio::Data::Cigar> revCigars{PacBio::Data::Cigar{"4="}};
    const std::vector<int32_t> fwdPositions{0};
    const std::vector<int32_t> revPositions{0};

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions));
}

TEST(Algorithm_Heteroduplex, empty_fwd_input_yields_no_call)
{
    const std::string reference{"ACGT"};
    const std::vector<std::string> fwdSeqs;
    const std::vector<std::string> revSeqs{"ACGT"};
    const std::vector<PacBio::Data::Cigar> fwdCigars;
    const std::vector<PacBio::Data::Cigar> revCigars{PacBio::Data::Cigar{"4="}};
    const std::vector<int32_t> fwdPositions;
    const std::vector<int32_t> revPositions{0};

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions));
}

TEST(Algorithm_Heteroduplex, empty_rev_input_yields_no_call)
{
    const std::string reference{"ACGT"};
    const std::vector<std::string> fwdSeqs{"ACGT"};
    const std::vector<std::string> revSeqs;
    const std::vector<PacBio::Data::Cigar> fwdCigars{PacBio::Data::Cigar{"4="}};
    const std::vector<PacBio::Data::Cigar> revCigars;
    const std::vector<int32_t> fwdPositions{0};
    const std::vector<int32_t> revPositions;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions));
}

TEST(Algorithm_Heteroduplex, no_call_on_mismatches_but_non_hd)
{
    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATATA",
        "GCTTACA",
        "GATTACA",
        "GCTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA",
        "GATTCA",
        "GATTA",
        "GATTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"2=1D2=1X1="},
        PacBio::Data::Cigar{"1=1X5="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"1=1X5="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"4=1D2="},
        PacBio::Data::Cigar{"4=2D1="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.0;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, call_simple_mismatch_heteroduplex_fwd)
{
    //         v
    // Ref: GATTACA
    // Fwd: GATCACA
    // Rev: GATTACA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTACA",
        "GATCACA",
        "GATCACA",
        "GATCACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA",
        "GATTACA",
        "GATTACA",
        "GATTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, call_simple_mismatch_heteroduplex_rev)
{
    //         v
    // Ref: GATTACA
    // Fwd: GATTACA
    // Rev: GATCACA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTACA",
        "GATTACA",
        "GATTACA",
        "GATTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA",
        "GATCACA",
        "GATCACA",
        "GATCACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, call_simple_del_heteroduplex_fwd)
{
    // Ref: GATTACA
    // Fwd: GATTA-A
    // Rev: GATCACA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTAA",
        "GATTAA",
        "GATTAA",
        "GATTAA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA",
        "GATTACA",
        "GATTACA",
        "GATTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"5=1D1="},
        PacBio::Data::Cigar{"5=1D1="},
        PacBio::Data::Cigar{"5=1D1="},
        PacBio::Data::Cigar{"5=1D1="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, call_simple_del_heteroduplex_rev)
{
    // Ref: GATTACA
    // Fwd: GATTACA
    // Rev: GATT-CA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTACA",
        "GATTACA",
        "GATTACA",
        "GATTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTAA",
        "GATTCA",
        "GATTCA",
        "GATTCA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"4=1D2="},
        PacBio::Data::Cigar{"4=1D2="},
        PacBio::Data::Cigar{"4=1D2="},
        PacBio::Data::Cigar{"4=1D2="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, skip_long_homopolymer_del_fwd)
{
    // Ref: GTTTTACA
    // Fwd: G-TTTACA
    // Rev: GTTTTACA

    const std::string reference{"GTTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTTACA",
        "GTTTACA",
        "GTTTACA",
        "GTTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"1=1D6="},
        PacBio::Data::Cigar{"1=1D6="},
        PacBio::Data::Cigar{"1=1D6="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"8="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, skip_long_homopolymer_del_rev)
{
    // Ref: GTTTTACA
    // Fwd: GTTTTACA
    // Rev: G-TTTACA

    const std::string reference{"GTTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GTTTACA",
        "GTTTACA",
        "GTTTTACA",
        "GTTTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"8="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"1=1D6="},
        PacBio::Data::Cigar{"1=1D6="},
        PacBio::Data::Cigar{"8="},
        PacBio::Data::Cigar{"1=1D6="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, merge_del_adjacent_to_short_homopolymer_fwd)
{
    // Ref: GTTTACA
    // Fwd: G-TTACA
    // Rev: GTTTACA

    const std::string reference{"GTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTACA",
        "GTTACA",
        "GTTACA",
        "GTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GTTTACA",
        "GTTTACA",
        "GTTTACA",
        "GTTTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"1=1D5="},
        PacBio::Data::Cigar{"1=1D5="},
        PacBio::Data::Cigar{"1=1D5="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, merge_del_adjacent_to_short_homopolymer_rev)
{
    // Ref: GTTTACA
    // Fwd: GTTTACA
    // Rev: G-TTACA

    const std::string reference{"GTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTACA",
        "GTTTACA",
        "GTTTACA",
        "GTTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GTTACA",
        "GTTACA",
        "GTTTACA",
        "GTTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"1=1D5="},
        PacBio::Data::Cigar{"1=1D5="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"1=1D5="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, snp_left_adjacent_of_del)
{
    // Ref: GTTTACA
    // Fwd: GTTT-CA
    // Rev: GTTAACA

    const std::string reference{"GTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTCA",
        "GTTTCA",
        "GTTTCA",
        "GTTTCA",
    };
    const std::vector<std::string> revSeqs{
        "GTTAACA",
        "GTTAACA",
        "GTTAACA",
        "GTTAACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"4=1D2="},
        PacBio::Data::Cigar{"4=1D2="},
        PacBio::Data::Cigar{"4=1D2="},
        PacBio::Data::Cigar{"4=1D2="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, snp_right_adjacent_of_del)
{
    // Ref: GTTTACA
    // Fwd: GTTTTCA
    // Rev: GTT-ACA

    const std::string reference{"GTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTTCA",
        "GTTTTCA",
        "GTTTTCA",
        "GTTTTCA",
    };
    const std::vector<std::string> revSeqs{
        "GTTACA",
        "GTTACA",
        "GTTACA",
        "GTTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"4=1X2="},
        PacBio::Data::Cigar{"4=1X2="},
        PacBio::Data::Cigar{"4=1X2="},
        PacBio::Data::Cigar{"4=1X2="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"3=1D3="},
        PacBio::Data::Cigar{"3=1D3="},
        PacBio::Data::Cigar{"3=1D3="},
        PacBio::Data::Cigar{"3=1D3="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, snp_right_adjacent_of_del_in_hompolymer)
{
    // Ref: GTTTTTACA
    // Fwd: G-TTTTACA
    // Rev: GTATTTACA

    const std::string reference{"GTTTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GTATTTACA",
        "GTATTTACA",
        "GTATTTACA",
        "GTATTTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"1=1D7="},
        PacBio::Data::Cigar{"1=1D7="},
        PacBio::Data::Cigar{"1=1D7="},
        PacBio::Data::Cigar{"1=1D7="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"2=1X6="},
        PacBio::Data::Cigar{"2=1X6="},
        PacBio::Data::Cigar{"2=1X6="},
        PacBio::Data::Cigar{"2=1X6="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, snp_left_adjacent_of_del_in_hompolymer)
{
    // Ref: GTTTTTACA
    // Fwd: GTTTT-ACA
    // Rev: GTTTTTGCA

    const std::string reference{"GTTTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
        "GTTTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GTTTTTGCA",
        "GTTTTTGCA",
        "GTTTTTGCA",
        "GTTTTTGCA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"5=1D3="},
        PacBio::Data::Cigar{"5=1D3="},
        PacBio::Data::Cigar{"5=1D3="},
        PacBio::Data::Cigar{"5=1D3="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"6=1X2="},
        PacBio::Data::Cigar{"6=1X2="},
        PacBio::Data::Cigar{"6=1X2="},
        PacBio::Data::Cigar{"6=1X2="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, returns_false_on_insufficient_coverage)
{
    //         v
    // Ref: GATTACA
    // Fwd: GATCACA
    // Rev: GATTACA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTACA",
        "GATCACA",
        "GATCACA",
        "GATCACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA",
        "GATTACA",
        "GATTACA",
        "GATTACA",
    };
    const std::vector<PacBio::Data::Cigar> fwdCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
        PacBio::Data::Cigar{"3=1X3="},
    };
    const std::vector<PacBio::Data::Cigar> revCigars{
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
        PacBio::Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // minimum coverage requirement is higher than provided
    Algorithm::HeteroduplexSettings settings;
    settings.MinimumPerStrandSubreadCoverage = 5;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(
        reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
        fwdPositions, revPositions, settings));
}

// clang-format on
