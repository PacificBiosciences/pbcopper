#include <pbcopper/data/Cigar.h>

#include <gtest/gtest.h>

using Cigar = PacBio::Data::Cigar;
using CigarOperation = PacBio::Data::CigarOperation;
using CigarOperationType = PacBio::Data::CigarOperationType;

// clang-format off

TEST(Data_CigarOperation, consumes_query)
{
    EXPECT_TRUE (ConsumesQuery(CigarOperationType::ALIGNMENT_MATCH));
    EXPECT_TRUE (ConsumesQuery(CigarOperationType::INSERTION));
    EXPECT_FALSE(ConsumesQuery(CigarOperationType::DELETION));
    EXPECT_FALSE(ConsumesQuery(CigarOperationType::REFERENCE_SKIP));
    EXPECT_TRUE (ConsumesQuery(CigarOperationType::SOFT_CLIP));
    EXPECT_FALSE(ConsumesQuery(CigarOperationType::HARD_CLIP));
    EXPECT_FALSE(ConsumesQuery(CigarOperationType::PADDING));
    EXPECT_TRUE (ConsumesQuery(CigarOperationType::SEQUENCE_MATCH));
    EXPECT_TRUE (ConsumesQuery(CigarOperationType::SEQUENCE_MISMATCH));
}

TEST(Data_CigarOperation, consumes_reference)
{
    EXPECT_TRUE (ConsumesReference(CigarOperationType::ALIGNMENT_MATCH));
    EXPECT_FALSE(ConsumesReference(CigarOperationType::INSERTION));
    EXPECT_TRUE (ConsumesReference(CigarOperationType::DELETION));
    EXPECT_TRUE (ConsumesReference(CigarOperationType::REFERENCE_SKIP));
    EXPECT_FALSE(ConsumesReference(CigarOperationType::SOFT_CLIP));
    EXPECT_FALSE(ConsumesReference(CigarOperationType::HARD_CLIP));
    EXPECT_FALSE(ConsumesReference(CigarOperationType::PADDING));
    EXPECT_TRUE (ConsumesReference(CigarOperationType::SEQUENCE_MATCH));
    EXPECT_TRUE (ConsumesReference(CigarOperationType::SEQUENCE_MISMATCH));
}

TEST(Data_CigarOperation, match_throw)
{
    // constexpr construction never throws
    EXPECT_NO_THROW({ [[maybe_unused]] constexpr CigarOperation cig('M', 1); });

    // runtime construction may throw
    EXPECT_THROW({ CigarOperation cig('M', 1); }, std::runtime_error);

    CigarOperation::DisableAutoValidation();
    EXPECT_NO_THROW({ CigarOperation cig('M', 1); });
    CigarOperation::EnableAutoValidation();
}

TEST(Data_CigarOperation, can_convert_enum_type_to_char)
{
    EXPECT_EQ('M', CigarOperation::TypeToChar(CigarOperationType::ALIGNMENT_MATCH));
    EXPECT_EQ('I', CigarOperation::TypeToChar(CigarOperationType::INSERTION));
    EXPECT_EQ('D', CigarOperation::TypeToChar(CigarOperationType::DELETION));
    EXPECT_EQ('N', CigarOperation::TypeToChar(CigarOperationType::REFERENCE_SKIP));
    EXPECT_EQ('S', CigarOperation::TypeToChar(CigarOperationType::SOFT_CLIP));
    EXPECT_EQ('H', CigarOperation::TypeToChar(CigarOperationType::HARD_CLIP));
    EXPECT_EQ('P', CigarOperation::TypeToChar(CigarOperationType::PADDING));
    EXPECT_EQ('=', CigarOperation::TypeToChar(CigarOperationType::SEQUENCE_MATCH));
    EXPECT_EQ('X', CigarOperation::TypeToChar(CigarOperationType::SEQUENCE_MISMATCH));
    EXPECT_EQ('?', CigarOperation::TypeToChar(CigarOperationType::UNKNOWN_OP));
}

TEST(Data_CigarOperation, can_convert_char_to_enum_type)
{
    EXPECT_EQ(CigarOperationType::ALIGNMENT_MATCH,   CigarOperation::CharToType('M'));
    EXPECT_EQ(CigarOperationType::INSERTION,         CigarOperation::CharToType('I'));
    EXPECT_EQ(CigarOperationType::DELETION,          CigarOperation::CharToType('D'));
    EXPECT_EQ(CigarOperationType::REFERENCE_SKIP,    CigarOperation::CharToType('N'));
    EXPECT_EQ(CigarOperationType::SOFT_CLIP,         CigarOperation::CharToType('S'));
    EXPECT_EQ(CigarOperationType::HARD_CLIP,         CigarOperation::CharToType('H'));
    EXPECT_EQ(CigarOperationType::PADDING,           CigarOperation::CharToType('P'));
    EXPECT_EQ(CigarOperationType::SEQUENCE_MATCH,    CigarOperation::CharToType('='));
    EXPECT_EQ(CigarOperationType::SEQUENCE_MISMATCH, CigarOperation::CharToType('X'));
    EXPECT_EQ(CigarOperationType::UNKNOWN_OP,        CigarOperation::CharToType('?'));
}

TEST(Data_CigarOperation, setting_via_enum_type_yields_expected_char)
{
    CigarOperation c1; c1.Type(CigarOperationType::ALIGNMENT_MATCH);
    CigarOperation c2; c2.Type(CigarOperationType::INSERTION);
    CigarOperation c3; c3.Type(CigarOperationType::DELETION);
    CigarOperation c4; c4.Type(CigarOperationType::REFERENCE_SKIP);
    CigarOperation c5; c5.Type(CigarOperationType::SOFT_CLIP);
    CigarOperation c6; c6.Type(CigarOperationType::HARD_CLIP);
    CigarOperation c7; c7.Type(CigarOperationType::PADDING);
    CigarOperation c8; c8.Type(CigarOperationType::SEQUENCE_MATCH);
    CigarOperation c9; c9.Type(CigarOperationType::SEQUENCE_MISMATCH);

    EXPECT_EQ('M', c1.Char());
    EXPECT_EQ('I', c2.Char());
    EXPECT_EQ('D', c3.Char());
    EXPECT_EQ('N', c4.Char());
    EXPECT_EQ('S', c5.Char());
    EXPECT_EQ('H', c6.Char());
    EXPECT_EQ('P', c7.Char());
    EXPECT_EQ('=', c8.Char());
    EXPECT_EQ('X', c9.Char());
}

TEST(Data_CigarOperation, setting_via_char_yields_expected_enum_type)
{
    CigarOperation c1; c1.Char('M');
    CigarOperation c2; c2.Char('I');
    CigarOperation c3; c3.Char('D');
    CigarOperation c4; c4.Char('N');
    CigarOperation c5; c5.Char('S');
    CigarOperation c6; c6.Char('H');
    CigarOperation c7; c7.Char('P');
    CigarOperation c8; c8.Char('=');
    CigarOperation c9; c9.Char('X');

    EXPECT_EQ(CigarOperationType::ALIGNMENT_MATCH,   c1.Type());
    EXPECT_EQ(CigarOperationType::INSERTION,         c2.Type());
    EXPECT_EQ(CigarOperationType::DELETION,          c3.Type());
    EXPECT_EQ(CigarOperationType::REFERENCE_SKIP,    c4.Type());
    EXPECT_EQ(CigarOperationType::SOFT_CLIP,         c5.Type());
    EXPECT_EQ(CigarOperationType::HARD_CLIP,         c6.Type());
    EXPECT_EQ(CigarOperationType::PADDING,           c7.Type());
    EXPECT_EQ(CigarOperationType::SEQUENCE_MATCH,    c8.Type());
    EXPECT_EQ(CigarOperationType::SEQUENCE_MISMATCH, c9.Type());
}

TEST(Data_Cigar, creates_empty_cigar_from_empty_string)
{
    const Cigar cigar = Cigar::FromStdString("");
    EXPECT_TRUE(cigar.empty());
}

TEST(Data_Cigar, can_create_from_single_op_string)
{
    const Cigar cigar = Cigar::FromStdString("100=");
    ASSERT_TRUE(cigar.size() == 1);

    const CigarOperation& op = cigar.front();
    EXPECT_TRUE(op.Char()   == '=');
    EXPECT_TRUE(op.Length() == 100);
}

TEST(Data_Cigar, can_create_from_multi_op_string)
{
    const Cigar cigar = Cigar::FromStdString("100=2D34I6=6X6=");
    ASSERT_TRUE(cigar.size() == 6);

    const CigarOperation& op0 = cigar.at(0);
    const CigarOperation& op1 = cigar.at(1);
    const CigarOperation& op2 = cigar.at(2);
    const CigarOperation& op3 = cigar.at(3);
    const CigarOperation& op4 = cigar.at(4);
    const CigarOperation& op5 = cigar.at(5);

    EXPECT_TRUE(op0.Char()   == '=');
    EXPECT_TRUE(op0.Length() == 100);
    EXPECT_TRUE(op1.Char()   == 'D');
    EXPECT_TRUE(op1.Length() == 2);
    EXPECT_TRUE(op2.Char()   == 'I');
    EXPECT_TRUE(op2.Length() == 34);
    EXPECT_TRUE(op3.Char()   == '=');
    EXPECT_TRUE(op3.Length() == 6);
    EXPECT_TRUE(op4.Char()   == 'X');
    EXPECT_TRUE(op4.Length() == 6);
    EXPECT_TRUE(op5.Char()   == '=');
    EXPECT_TRUE(op5.Length() == 6);
}

TEST(Data_Cigar, creates_empty_string_from_empty_cigar)
{
    const std::string empty;
    const Cigar cigar;
    EXPECT_EQ(empty, cigar.ToStdString());
}

TEST(Data_Cigar, can_create_string_from_single_op_cigar)
{
    Cigar cigar;
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH, 100);
    EXPECT_EQ("100=", cigar.ToStdString());
}

TEST(Data_Cigar, can_create_string_from_multi_op_cigar)
{
    Cigar cigar;
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH,  100);
    cigar.emplace_back(CigarOperationType::DELETION,          2);
    cigar.emplace_back(CigarOperationType::INSERTION,        34);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH,    6);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MISMATCH, 6);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH,    6);

    EXPECT_EQ("100=2D34I6=6X6=", cigar.ToStdString());
}

TEST(Data_CigarOpsCalculator, can_count_base_events){
 const Cigar cigar = Cigar::FromStdString("5=1X2D3I");
 const auto results = CigarOpsCalculator(cigar);

 EXPECT_EQ(results.DeletionBases, 2);
 EXPECT_EQ(results.InsertionBases, 3);
 EXPECT_EQ(results.MatchBases, 5);
 EXPECT_EQ(results.MismatchBases, 1);


}

// this tests the number of operations is correct, not base counts
TEST(Data_CigarOpsCalculator, can_count_cigar_opts){
 const Cigar cigar = Cigar::FromStdString("1X2=2X2D2I1D1=1X1=1X");
 const auto results = CigarOpsCalculator(cigar);

 EXPECT_EQ(results.DeletionEvents, 2);
 EXPECT_EQ(results.InsertionEvents, 1);
 EXPECT_EQ(results.MatchEvents, 3);
 EXPECT_EQ(results.MismatchEvents, 4);
}

TEST(Data_CigarOpsCalculator, can_calculate_identity){
 const Cigar cigar = Cigar::FromStdString("100=10D100=");
 const auto results = CigarOpsCalculator(cigar);

 // 200/210
 EXPECT_NEAR(results.Identity, 95.2381, 0.01 );
}

TEST(Data_CigarOpsCalculator, can_calculate_gap_compressed_identity_with_deletions){
 const Cigar cigar = Cigar::FromStdString("100=10D100=");
 const auto results = CigarOpsCalculator(cigar);

 // 200/201
 EXPECT_NEAR(results.GapCompressedIdentity, 99.50249, 0.01);
}

TEST(Data_CigarOpsCalculator, can_calculate_gap_compressed_identity_with_insertions){
 const Cigar cigar = Cigar::FromStdString("100=10I100=");
 const auto results = CigarOpsCalculator(cigar);

 // 200/201
 EXPECT_NEAR(results.GapCompressedIdentity, 99.50249, 0.01);
}

// clang-format off

TEST(Data_Cigar, can_convert_cigar_to_m5)
{
    struct TestData {
        std::string reference;
        int32_t rStart;
        int32_t rEnd;
        std::string query;
        int32_t qStart;
        int32_t qEnd;
        Cigar cigar;
        std::string expectedReference;
        std::string expectedQuery;
        bool queryReversed;
        bool expectedReturnValue;
    };

    const std::vector<std::pair<std::string, TestData>> tests{
        std::make_pair("empty input",
            TestData{
                "", 0, 0,           // input reference (begin, end)
                "", 0, 0,           // input query (begin, end)
                "",                 // cigar
                "",                 // expected reference
                "",                 // expected query
                false,              // query is reverse complemented
                false               // expected method return value
            }),
        std::make_pair("empty reference",
            TestData{
                "",      0, 0,
                "ACGT",  0, 4,
                "4I",
                "----",
                "ACGT",
                 false,
                 true
            }),
        std::make_pair("empty query",
            TestData{
                "ACTG", 0, 4,
                "",     0, 0,
                "4D",
                "ACTG",
                "----",
                false,
                true
            }),
        std::make_pair("empty CIGAR",
            TestData{
                "ACTG", 0, 4,
                "ACTG", 0, 4,
                "",
                "",
                "",
                false,
                false
            }),
        std::make_pair("CIGAR not valid for inputs",
            TestData{
                "ACTG", 0, 4,
                "ACTG", 0, 4,
                "3=",
                "",
                "",
                false,
                false
            }),
        std::make_pair("simple exact match",
            TestData{
                "ACTG", 0, 4,
                "ACTG", 0, 4,
                "4=",
                "ACTG",
                "ACTG",
                false,
                true
            }),
        std::make_pair("mix of matches, mismatches, and indels",
            TestData{
                "AAAAAAAAAAA",  0, 11,
                "AAAAAAAA",     0, 8,
                "3=2D2=1I2D2=",
                "AAAAAAA-AAAA",
                "AAA--AAA--AA",
                false,
                true
            }),
        std::make_pair("conversion internal to sequences",
            TestData{
                "AAAACCCCCCCCCCTTT", 4, 14,
                "GCCCCCCCCCCG",      1, 11,
                "10=",
                "CCCCCCCCCC",
                "CCCCCCCCCC",
                false,
                true
            }),
        std::make_pair("reverse complement",
            TestData{
                "AAAACCCCCCCCCCTTT", 4, 14,
                "CGGGGGGGGGGC",      1, 11,
                "10=",
                "CCCCCCCCCC",
                "CCCCCCCCCC",
                true,
                true
            }),
    };

    for (const auto& test : tests) {
        SCOPED_TRACE(test.first);
        const auto& testData = test.second;

        std::string alignedReference;
        std::string alignedQuery;
        const auto returnValue = PacBio::Data::ConvertCigarToM5(
            testData.reference, testData.query,
            testData.rStart, testData.rEnd,
            testData.qStart, testData.qEnd,
            testData.queryReversed, testData.cigar,
            alignedReference, alignedQuery
        );

        EXPECT_EQ(returnValue, testData.expectedReturnValue);
        EXPECT_EQ(alignedReference, testData.expectedReference);
        EXPECT_EQ(alignedQuery, testData.expectedQuery);
    }
}

TEST(Data_Cigar, can_validate)
{
    // empty
    EXPECT_TRUE(PacBio::Data::ValidateCigar("", "", ""));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("", "A", ""));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("", "", "A"));

    // adjacent
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1I2I", "", "AAA"));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("3D2D", "AAAAA", ""));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1N4N", "AAAA", ""));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("2S2S", "", "AAAA"));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1H1H", "", ""));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("5P3P", "", ""));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1=1=", "AA", "AA"));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1X1X", "AA", "CC"));

    // hard clip
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1=1H1=", "AA", "AA"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1H1=", "A", "A"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1=1H", "A", "A"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1H1X1H", "A", "C"));

    // soft clip
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1=1S1=", "AA", "AA"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1S1X", "A", "AC"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1X1S", "A", "CA"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1S1D1I1S", "A", "ACA"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1H1S1D1I1S1H", "A", "ACA"));

    // length
    EXPECT_FALSE(PacBio::Data::ValidateCigar("3=", "AA", "AAA"));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1S3D3=", "CCCAAA", "AAA"));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1D2X", "AC", "CA"));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1H1I1D", "A", ""));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("1H1P2N2S3H", "A", "CC"));

    // content
    EXPECT_FALSE(PacBio::Data::ValidateCigar("3=", "AAC", "AAA"));
    EXPECT_FALSE(PacBio::Data::ValidateCigar("3X", "ACG", "AAA"));
    EXPECT_TRUE(PacBio::Data::ValidateCigar("1H1S2=2N2X1D1I2P3S5H", "AACCCCC", "CAAGGGTTT"));
}

TEST(Data_Cigar, can_left_align)
{
    std::string target1{"AAA"};
    std::string query1{"AAA"};
    Cigar cigarIn1{"3="};
    Cigar cigarOut1{"3="};

    // can replace mismatches
    // T: AAA    A-A-A-
    //        ->
    // Q: TTT    -T-T-T
    std::string target2{"AAA"};
    std::string query2{"TTT"};
    Cigar cigarIn2{"3X"};
    Cigar cigarOut2{"3X"};
    Cigar cigarOut2X{"1D1I1D1I1D1I"};

    // should remove padding
    // T: AA-A    AAA
    //    || | -> |||
    // Q: AA*A    AAA
    std::string target3{"AAA"};
    std::string query3{"AAA"};
    Cigar cigarIn3{"2=1P1="};
    Cigar cigarOut3{"3="};

    // should not modify soft clip
    // T: -AAA--    -AAA--
    //    #| |## -> #|| ##
    // Q: AA-AAA    AAA-AA
    std::string target4{"AAA"};
    std::string query4{"AAAAA"};
    Cigar cigarIn4{"1S1=1D1=2S"};
    Cigar cigarOut4{"1S2=1D2S"};

    // should not modify hard clip
    // T:  AAA      AAA
    //       |  ->  |
    // Q: #--A#    #A--#
    std::string target5{"AAA"};
    std::string query5{"A"};
    Cigar cigarIn5{"3H2D1=5H"};
    Cigar cigarOut5{"3H1=2D5H"};

    // should replace mismatches with deletions first
    // T: AAA--CA--T-TGG    AAAC--AT---TGG    AAAC--ATTGG--
    //    | |            -> || |   |       or || |   ||||
    // Q: A-ACCTTTGGGG--    AA-CCTTTGGGG--    AA-CCT-TTGGGG
    std::string target6{"AAACATTGG"};
    std::string query6{"AACCTTTGGGG"};
    Cigar cigarIn6{"1=1D1=2I2X2I1X1I1X2D"};
    Cigar cigarOut6{"2=1D1=2I1X1=3I1X2D"};
    Cigar cigarOut6X{"2=1D1=2I1D4=2I"};

    // same sequences in different order can yield different result while replacing mismatches
    // T: A-ACCTTTGGGG--    AA-CCTTTGGGG--    AA-CCTT-TGGGG---
    //    | |            -> || |   |       or || |    |
    // Q: AAA--CA--T-TGG    AAAC--AT---TGG    AAAC---AT----TGG
    std::string target7{"AACCTTTGGGG"};
    std::string query7{"AAACATTGG"};
    Cigar cigarIn7{"1=1I1=2D2X2D1X1D1X2I"};
    Cigar cigarOut7{"2=1I1=2D1X1=3D1X2I"};
    Cigar cigarOut7X{"2=1I1=3D1I1=4D3I"};

    // ---AAACCC------TTT    AAACCCTTT
    //                    -> |||||||||
    // AAA------CCCTTT---    AAACCCTTT
    std::string target8{"AAACCCTTT"};
    std::string query8{"AAACCCTTT"};
    Cigar cigarIn8{"3I6D6I3D"};
    Cigar cigarOut8{"9="};

    // clang-format off

    EXPECT_EQ(cigarOut1,  PacBio::Data::LeftAlignCigar(cigarIn1, target1, query1));
    EXPECT_EQ(cigarOut1,  PacBio::Data::LeftAlignCigar(cigarIn1, target1, query1, true));
    EXPECT_EQ(cigarOut2,  PacBio::Data::LeftAlignCigar(cigarIn2, target2, query2));
    EXPECT_EQ(cigarOut2X, PacBio::Data::LeftAlignCigar(cigarIn2, target2, query2, true));
    EXPECT_EQ(cigarOut3,  PacBio::Data::LeftAlignCigar(cigarIn3, target3, query3));
    EXPECT_EQ(cigarOut4,  PacBio::Data::LeftAlignCigar(cigarIn4, target4, query4));
    EXPECT_EQ(cigarOut5,  PacBio::Data::LeftAlignCigar(cigarIn5, target5, query5));
    EXPECT_EQ(cigarOut6,  PacBio::Data::LeftAlignCigar(cigarIn6, target6, query6));
    EXPECT_EQ(cigarOut6X, PacBio::Data::LeftAlignCigar(cigarIn6, target6, query6, true));
    EXPECT_EQ(cigarOut7,  PacBio::Data::LeftAlignCigar(cigarIn7, target7, query7));
    EXPECT_EQ(cigarOut7X, PacBio::Data::LeftAlignCigar(cigarIn7, target7, query7, true));
    EXPECT_EQ(cigarOut8,  PacBio::Data::LeftAlignCigar(cigarIn8, target8, query8));

    // clang-format on
}

// clang-format on
