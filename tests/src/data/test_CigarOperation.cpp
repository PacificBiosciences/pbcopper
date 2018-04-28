// Author: Derek Barnett

#include <gtest/gtest.h>
#include <pbcopper/data/CigarOperation.h>
#include <string>

using PacBio::Data::CigarOperation;
using PacBio::Data::CigarOperationType;

TEST(Data_CigarOperation, can_convert_type_to_char)
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

TEST(Data_CigarOperation, can_convert_char_to_type)
{
    EXPECT_EQ(CigarOperationType::ALIGNMENT_MATCH, CigarOperation::CharToType('M'));
    EXPECT_EQ(CigarOperationType::INSERTION, CigarOperation::CharToType('I'));
    EXPECT_EQ(CigarOperationType::DELETION, CigarOperation::CharToType('D'));
    EXPECT_EQ(CigarOperationType::REFERENCE_SKIP, CigarOperation::CharToType('N'));
    EXPECT_EQ(CigarOperationType::SOFT_CLIP, CigarOperation::CharToType('S'));
    EXPECT_EQ(CigarOperationType::HARD_CLIP, CigarOperation::CharToType('H'));
    EXPECT_EQ(CigarOperationType::PADDING, CigarOperation::CharToType('P'));
    EXPECT_EQ(CigarOperationType::SEQUENCE_MATCH, CigarOperation::CharToType('='));
    EXPECT_EQ(CigarOperationType::SEQUENCE_MISMATCH, CigarOperation::CharToType('X'));
}

TEST(Data_CigarOperation, can_set_via_type)
{
    CigarOperation c2;
    c2.Type(CigarOperationType::INSERTION);
    CigarOperation c3;
    c3.Type(CigarOperationType::DELETION);
    CigarOperation c4;
    c4.Type(CigarOperationType::REFERENCE_SKIP);
    CigarOperation c5;
    c5.Type(CigarOperationType::SOFT_CLIP);
    CigarOperation c6;
    c6.Type(CigarOperationType::HARD_CLIP);
    CigarOperation c7;
    c7.Type(CigarOperationType::PADDING);
    CigarOperation c8;
    c8.Type(CigarOperationType::SEQUENCE_MATCH);
    CigarOperation c9;
    c9.Type(CigarOperationType::SEQUENCE_MISMATCH);

    EXPECT_EQ('I', c2.Char());
    EXPECT_EQ('D', c3.Char());
    EXPECT_EQ('N', c4.Char());
    EXPECT_EQ('S', c5.Char());
    EXPECT_EQ('H', c6.Char());
    EXPECT_EQ('P', c7.Char());
    EXPECT_EQ('=', c8.Char());
    EXPECT_EQ('X', c9.Char());
}

TEST(Data_CigarOperation, can_set_via_char)
{
    CigarOperation c2;
    c2.Char('I');
    CigarOperation c3;
    c3.Char('D');
    CigarOperation c4;
    c4.Char('N');
    CigarOperation c5;
    c5.Char('S');
    CigarOperation c6;
    c6.Char('H');
    CigarOperation c7;
    c7.Char('P');
    CigarOperation c8;
    c8.Char('=');
    CigarOperation c9;
    c9.Char('X');

    EXPECT_EQ(CigarOperationType::INSERTION, c2.Type());
    EXPECT_EQ(CigarOperationType::DELETION, c3.Type());
    EXPECT_EQ(CigarOperationType::REFERENCE_SKIP, c4.Type());
    EXPECT_EQ(CigarOperationType::SOFT_CLIP, c5.Type());
    EXPECT_EQ(CigarOperationType::HARD_CLIP, c6.Type());
    EXPECT_EQ(CigarOperationType::PADDING, c7.Type());
    EXPECT_EQ(CigarOperationType::SEQUENCE_MATCH, c8.Type());
    EXPECT_EQ(CigarOperationType::SEQUENCE_MISMATCH, c9.Type());
}

TEST(Data_CigarOperation, throws_on_invalid_op)
{
#ifdef PBCOPPER_PERMISSIVE_CIGAR

    // ctor(c, len)
    EXPECT_NO_THROW(CigarOperation('M', 1));

    // ctor(enum, len)
    EXPECT_NO_THROW(CigarOperation(CigarOperationType::ALIGNMENT_MATCH, 1));

    // setter via enum
    EXPECT_NO_THROW({
        CigarOperation c;
        c.Type(CigarOperationType::ALIGNMENT_MATCH);
    });

    // setter via char code
    EXPECT_NO_THROW({
        CigarOperation c;
        c.Char('M');
    });

#else

    // ctor(c, len)
    EXPECT_THROW({ CigarOperation('M', 1); }, std::runtime_error);

    // ctor(enum, len)
    EXPECT_THROW({ CigarOperation(CigarOperationType::ALIGNMENT_MATCH, 1); }, std::runtime_error);

    // setter via enum
    EXPECT_THROW(
        {
            CigarOperation c;
            c.Type(CigarOperationType::ALIGNMENT_MATCH);
        },
        std::runtime_error);

    // setter via char code
    EXPECT_THROW(
        {
            CigarOperation c;
            c.Char('M');
        },
        std::runtime_error);

#endif
}

TEST(Data_CigarOperation, input_operator)
{
    const std::string s{"42="};
    std::istringstream in{s};

    CigarOperation op;
    in >> op;

    EXPECT_EQ(42, op.Length());
    EXPECT_EQ('=', op.Char());
    EXPECT_EQ(CigarOperationType::SEQUENCE_MATCH, op.Type());
}

TEST(Data_CigarOperation, output_operator)
{
    const CigarOperation op{'=', 42};

    std::ostringstream out;
    out << op;

    EXPECT_EQ("42=", out.str());
}
