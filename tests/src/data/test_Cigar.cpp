#include <gtest/gtest.h>

#include <pbcopper/data/Cigar.h>

using Cigar = PacBio::Data::Cigar;
using CigarOperation = PacBio::Data::CigarOperation;
using CigarOperationType = PacBio::Data::CigarOperationType;

// clang-format off

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

    const CigarOperation op = cigar.front();
    EXPECT_TRUE(op.Char()   == '=');
    EXPECT_TRUE(op.Length() == 100);
}

TEST(Data_Cigar, can_create_from_multi_op_string)
{
    const Cigar cigar = Cigar::FromStdString("100=2D34I6=6X6=");
    ASSERT_TRUE(cigar.size() == 6);

    const CigarOperation op0 = cigar.at(0);
    const CigarOperation op1 = cigar.at(1);
    const CigarOperation op2 = cigar.at(2);
    const CigarOperation op3 = cigar.at(3);
    const CigarOperation op4 = cigar.at(4);
    const CigarOperation op5 = cigar.at(5);

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
    cigar.push_back( CigarOperation(CigarOperationType::SEQUENCE_MATCH, 100) );
    EXPECT_EQ("100=", cigar.ToStdString());
}

TEST(Data_Cigar, can_create_string_from_multi_op_cigar)
{
    Cigar cigar;
    cigar.push_back(CigarOperation(CigarOperationType::SEQUENCE_MATCH,  100));
    cigar.push_back(CigarOperation(CigarOperationType::DELETION,          2));
    cigar.push_back(CigarOperation(CigarOperationType::INSERTION,        34));
    cigar.push_back(CigarOperation(CigarOperationType::SEQUENCE_MATCH,    6));
    cigar.push_back(CigarOperation(CigarOperationType::SEQUENCE_MISMATCH, 6));
    cigar.push_back(CigarOperation(CigarOperationType::SEQUENCE_MATCH,    6));

    EXPECT_EQ("100=2D34I6=6X6=", cigar.ToStdString());
}

// clang-format on
