#include <pbcopper/algorithm/Heteroduplex.h>
#include <pbcopper/algorithm/internal/HeteroduplexUtils.h>

#include <pbcopper/utility/Ssize.h>

#include <gtest/gtest.h>

#include <algorithm>

using namespace PacBio;

// clang-format off

TEST(Algorithm_Heteroduplex, can_determine_most_common_base) 
{
    const Algorithm::internal::BaseCount bc{2,4,0,1,1};
    const auto mbc = Algorithm::internal::MostCommonBase(bc, 'A');
    EXPECT_EQ('C', mbc.first);
    EXPECT_EQ(4, mbc.second);
}

TEST(Algorithm_Heteroduplex, most_common_base_is_ref_for_tie) 
{
    const Algorithm::internal::BaseCount bc{4,4,0,1,1};
    const auto mbc = Algorithm::internal::MostCommonBase(bc, 'A');
    EXPECT_EQ('A', mbc.first);
    EXPECT_EQ(4, mbc.second);  
}

TEST(Algorithm_Heteroduplex, most_common_base_is_ref_for_no_coverage) 
{
    // should not happen, but stranger things have
    const Algorithm::internal::BaseCount bc{0,0,0,0,0};
    const auto mbc = Algorithm::internal::MostCommonBase(bc, 'A');
    EXPECT_EQ('A', mbc.first);
    EXPECT_EQ(0, mbc.second); 
}

TEST(Algorithm_Heteroduplex, empty_strand_raw_data_from_empty_input)
{
    const std::string reference;
    const std::vector<std::string> seqs;
    const std::vector<Data::Cigar> cigars;
    const std::vector<int32_t> positions;

    const auto rawData = 
        Algorithm::internal::CalculateStrandRawData(
            reference, {seqs, cigars, positions});

    EXPECT_TRUE(rawData.NumReads.empty());
    EXPECT_TRUE(rawData.BaseCounts.empty());
    EXPECT_TRUE(rawData.PotentialMismatches.empty());
    EXPECT_TRUE(rawData.Insertions.empty());
}

TEST(Algorithm_Heteroduplex, strand_raw_data_from_perfect_match_cigar_counts)
{
    // input

    // mm:  00000000000000
    // ref: GATTACAGATTACA
    // -----------------------
    //      GATTACAGATTACA   
    //       ATTACAGATTAC
    //      GATTACAGATTA
    //        TTACAGATTACA

    const std::string reference{
        "GATTACAGATTACA"
    };
    const std::vector<std::string> inputSeqs{
        "GATTACAGATTACA",
         "ATTACAGATTAC",
        "GATTACAGATTA",
          "TTACAGATTACA",
    };
    const std::vector<Data::Cigar> inputCigars{
        Data::Cigar{"14="},
        Data::Cigar{"12="},
        Data::Cigar{"12="},
        Data::Cigar{"12="},
    };
    const std::vector<int32_t> inputPositions{0, 1, 0, 2};

    // expected

    const std::vector<int> expectedNumReads{
        2,3,4,4,4,4,4,4,4,4,4,4,3,2
    };
    const std::vector<uint8_t> expectedPotentialMismatches{
        0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    const std::vector<Algorithm::internal::BaseCount> expectedBaseCounts{
        {0,0,2,0,0}, {3,0,0,0,0}, {0,0,0,4,0}, {0,0,0,4,0}, {4,0,0,0,0},
        {0,4,0,0,0}, {4,0,0,0,0}, {0,0,4,0,0}, {4,0,0,0,0}, {0,0,0,4,0},
        {0,0,0,4,0}, {4,0,0,0,0}, {0,3,0,0,0}, {2,0,0,0,0},
    };
    const std::vector<std::vector<std::string_view>> expectedInsertions{
        {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    };

    // test

    const auto rawData =
        Algorithm::internal::CalculateStrandRawData(
            reference, {inputSeqs, inputCigars, inputPositions});
    
    EXPECT_EQ(expectedNumReads, rawData.NumReads);
    EXPECT_EQ(expectedPotentialMismatches, rawData.PotentialMismatches);
    EXPECT_EQ(rawData.BaseCounts, expectedBaseCounts);
    EXPECT_EQ(rawData.Insertions, expectedInsertions);
}

TEST(Algorithm_Heteroduplex, strand_raw_data_with_mismatches_and_indels)
{
    // input

    // mm:  00 0 10101010000
    // ref: GA-TTACAGATTACA
    // -----------------------
    //      GAATTACAAATTACA
    //       A-TAACAGATTAC
    //      GA-TTACAGA-TA
    //         TTA-AGATTACA

    const std::string reference{
        "GATTACAGATTACA"
    };
    const std::vector<std::string> inputSeqs{
        "GAATTACAAATTACA",
        "ATAACAGATTAC",
        "GATTACAGATA",
        "TTAAGATTACA",
    };
    const std::vector<Data::Cigar> inputCigars{
        Data::Cigar{"2=1I5=1X6="},
        Data::Cigar{"2=1X9="},
        Data::Cigar{"9=1D2="},
        Data::Cigar{"3=1D8="},
    };
    const std::vector<int32_t> inputPositions{0, 1, 0, 2};

    // expected

    const std::vector<int> expectedNumReads{
        2,3,4,4,4,4,4,4,4,4,4,4,3,2
    };
    const std::vector<uint8_t> expectedPotentialMismatches{
        0,0,0,1,0,1,0,1,0,1,0,0,0,0,
    };

    const std::vector<Algorithm::internal::BaseCount> expectedBaseCounts{
        {0,0,2,0,0}, {3,0,0,0,0}, {0,0,0,4,0}, {1,0,0,3,0}, {4,0,0,0,0},
        {0,3,0,0,1}, {4,0,0,0,0}, {1,0,3,0,0}, {4,0,0,0,0}, {0,0,0,3,1},
        {0,0,0,4,0}, {4,0,0,0,0}, {0,3,0,0,0}, {2,0,0,0,0},
    };
    const std::vector<std::vector<std::string_view>> expectedInsertions{
        {}, {}, {"A"}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
    };

    const auto rawData =
        Algorithm::internal::CalculateStrandRawData(
            reference, {inputSeqs, inputCigars, inputPositions});

    EXPECT_EQ(expectedNumReads, rawData.NumReads);
    EXPECT_EQ(expectedPotentialMismatches, rawData.PotentialMismatches);
    EXPECT_EQ(rawData.BaseCounts, expectedBaseCounts);
    EXPECT_EQ(rawData.Insertions, expectedInsertions);
}

TEST(Algorithm_Heteroduplex, empty_reference_yields_no_call)
{
    const std::string reference;
    const std::vector<std::string> fwdSeqs{"ACGT"};
    const std::vector<std::string> revSeqs{"ACGT"};
    const std::vector<Data::Cigar> fwdCigars{Data::Cigar{"4="}};
    const std::vector<Data::Cigar> revCigars{Data::Cigar{"4="}};
    const std::vector<int32_t> fwdPositions{0};
    const std::vector<int32_t> revPositions{0};

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                           fwdPositions, revPositions));
}

TEST(Algorithm_Heteroduplex, empty_fwd_input_yields_no_call)
{
    const std::string reference{"ACGT"};
    const std::vector<std::string> fwdSeqs;
    const std::vector<std::string> revSeqs{"ACGT"};
    const std::vector<Data::Cigar> fwdCigars;
    const std::vector<Data::Cigar> revCigars{Data::Cigar{"4="}};
    const std::vector<int32_t> fwdPositions;
    const std::vector<int32_t> revPositions{0};

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                           fwdPositions, revPositions));
}

TEST(Algorithm_Heteroduplex, empty_rev_input_yields_no_call)
{
    const std::string reference{"ACGT"};
    const std::vector<std::string> fwdSeqs{"ACGT"};
    const std::vector<std::string> revSeqs;
    const std::vector<Data::Cigar> fwdCigars{Data::Cigar{"4="}};
    const std::vector<Data::Cigar> revCigars;
    const std::vector<int32_t> fwdPositions{0};
    const std::vector<int32_t> revPositions;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                           fwdPositions, revPositions));
}

TEST(Algorithm_Heteroduplex, returns_false_on_insufficient_coverage)
{
    //         v
    // Ref: GATTACA
    // Fwd: GATCACA
    // Rev: GATTACA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTACA", "GATCACA", "GATCACA", "GATCACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA", "GATTACA", "GATTACA", "GATTACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"7="}, Data::Cigar{"3=1X3="}, Data::Cigar{"3=1X3="},
        Data::Cigar{"3=1X3="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"7="}, Data::Cigar{"7="}, Data::Cigar{"7="},
        Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // minimum coverage requirement is higher than provided
    Algorithm::HeteroduplexSettings settings;
    settings.MinimumPerStrandSubreadCoverage = 5;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                           fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, no_call_on_mismatches_but_non_hd)
{
    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATATA", "GCTTACA", "GATTACA", "GCTTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA", "GATTCA", "GATTA", "GATTACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"2=1D2=1X1="}, Data::Cigar{"1=1X5="}, Data::Cigar{"7="},
        Data::Cigar{"1=1X5="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"7="}, Data::Cigar{"4=1D2="}, Data::Cigar{"4=2D1="},
        Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.0;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
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
        "GATTACA", "GATCACA", "GATCACA", "GATCACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA", "GATTACA", "GATTACA", "GATTACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"7="}, Data::Cigar{"3=1X3="}, Data::Cigar{"3=1X3="},
        Data::Cigar{"3=1X3="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"7="}, Data::Cigar{"7="}, Data::Cigar{"7="},
        Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
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
        "GATTACA", "GATTACA", "GATTACA", "GATTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA", "GATCACA", "GATCACA", "GATCACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"7="}, Data::Cigar{"7="}, Data::Cigar{"7="},
        Data::Cigar{"7="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"7="}, Data::Cigar{"3=1X3="}, Data::Cigar{"3=1X3="},
        Data::Cigar{"3=1X3="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                          fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, skip_deletion_fwd)
{
    //           v
    // Ref: GATTACA
    // Fwd: GATTA-A
    // Rev: GATCACA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTAA", "GATTAA", "GATTAA", "GATTAA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA", "GATTACA", "GATTACA", "GATTACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"5=1D1="}, Data::Cigar{"5=1D1="}, Data::Cigar{"5=1D1="},
        Data::Cigar{"5=1D1="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"7="}, Data::Cigar{"7="}, Data::Cigar{"7="},
        Data::Cigar{"7="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset, with most permissive alpha to ensure no 
    // mismatch gets that far
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.0;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                          fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, skip_deletion_rev)
{
    //          v
    // Ref: GATTACA
    // Fwd: GATTACA
    // Rev: GATT-CA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTACA", "GATTACA", "GATTACA", "GATTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTAA", "GATTCA", "GATTCA", "GATTCA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"7="}, Data::Cigar{"7="}, Data::Cigar{"7="},
        Data::Cigar{"7="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"4=1D2="}, Data::Cigar{"4=1D2="}, Data::Cigar{"4=1D2="},
        Data::Cigar{"4=1D2="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset, with most permissive alpha to ensure no 
    // mismatch gets that far
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.0;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                          fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, call_snp_left_adjacent_of_del)
{
    //         vv
    // Ref: GTCTACA
    // Fwd: GTCT-CA
    // Rev: GTCAACA

    const std::string reference{"GTCTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTCTCA", "GTCTCA", "GTCTCA", "GTCTCA",
    };
    const std::vector<std::string> revSeqs{
        "GTCAACA", "GTCAACA", "GTCAACA", "GTCAACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"4=1D2="}, Data::Cigar{"4=1D2="}, Data::Cigar{"4=1D2="},
        Data::Cigar{"4=1D2="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"3=1X3="}, Data::Cigar{"3=1X3="}, Data::Cigar{"3=1X3="},
        Data::Cigar{"3=1X3="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                          fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, call_snp_right_adjacent_of_del)
{
    //         vv
    // Ref: GTTTACA
    // Fwd: GTTTTCA
    // Rev: GTT-ACA

    const std::string reference{"GTTTACA"};
    const std::vector<std::string> fwdSeqs{
        "GTTTTCA", "GTTTTCA", "GTTTTCA", "GTTTTCA",
    };
    const std::vector<std::string> revSeqs{
        "GTTACA", "GTTACA", "GTTACA", "GTTACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"4=1X2="}, Data::Cigar{"4=1X2="}, Data::Cigar{"4=1X2="},
        Data::Cigar{"4=1X2="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"3=1D3="}, Data::Cigar{"3=1D3="}, Data::Cigar{"3=1D3="},
        Data::Cigar{"3=1D3="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjacent for small dataset
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.1;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_TRUE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                          fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, skip_overhang_snp_fwd)
{
    //         v
    // Ref: GATTACA
    // Fwd: GATTACA
    // Rev: GATAACA

    const std::string reference{"GATTACA"};
    const std::vector<std::string> fwdSeqs{
        "GATTACA", "GATTACA", "GATTACA", "GATTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATTACA", "GATAACA", "GATAACA", "GATAACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"7="}, Data::Cigar{"7="}, Data::Cigar{"7="},
        Data::Cigar{"7="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"7="}, Data::Cigar{"3=1X3="}, Data::Cigar{"3=1X3="},
        Data::Cigar{"3=1X3="},
    };
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset, with most permissive alpha to ensure no 
    // mismatch gets that far
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.0;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                          fwdPositions, revPositions, settings));
}

TEST(Algorithm_Heteroduplex, skip_overhang_snp_rev)
{
    //         v
    // Ref: GATAACA
    // Fwd: GATTACA
    // Rev: GATAACA

    const std::string reference{"GATAACA"};
    const std::vector<std::string> fwdSeqs{
        "GATAACA", "GATTACA", "GATTACA", "GATTACA",
    };
    const std::vector<std::string> revSeqs{
        "GATAACA", "GATAACA", "GATAACA", "GATAACA",
    };
    const std::vector<Data::Cigar> fwdCigars{
        Data::Cigar{"7="}, Data::Cigar{"3=1X3="}, Data::Cigar{"3=1X3="},
        Data::Cigar{"3=1X3="},
    };
    const std::vector<Data::Cigar> revCigars{
        Data::Cigar{"7="}, Data::Cigar{"7="}, Data::Cigar{"7="},
        Data::Cigar{"7="},
    };  
    const std::vector<int32_t> fwdPositions{0, 0, 0, 0};
    const std::vector<int32_t> revPositions{0, 0, 0, 0};

    // adjust for small dataset, with most permissive alpha to ensure no 
    // mismatch gets that far
    Algorithm::HeteroduplexSettings settings;
    settings.IgnoreEndBases = 0;
    settings.AlphaLevel = 0.0;
    settings.MinimumPerStrandSubreadCoverage = 4;

    EXPECT_FALSE(Algorithm::IsHeteroduplex(reference, fwdSeqs, revSeqs, fwdCigars, revCigars,
                                          fwdPositions, revPositions, settings));
}

// clang-format on
