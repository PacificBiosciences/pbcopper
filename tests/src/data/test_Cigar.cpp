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

// clang-format on
