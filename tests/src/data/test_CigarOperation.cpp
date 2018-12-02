// Author: Derek Barnett
#include <pbcopper/data/CigarOperation.h>

#include <string>

#include <gtest/gtest.h>

TEST(Data_CigarOperation, can_convert_type_to_char)
{
    EXPECT_EQ('M', PacBio::Data::CigarOperation::TypeToChar(
                       PacBio::Data::CigarOperationType::ALIGNMENT_MATCH));
    EXPECT_EQ(
        'I', PacBio::Data::CigarOperation::TypeToChar(PacBio::Data::CigarOperationType::INSERTION));
    EXPECT_EQ('D',
              PacBio::Data::CigarOperation::TypeToChar(PacBio::Data::CigarOperationType::DELETION));
    EXPECT_EQ('N', PacBio::Data::CigarOperation::TypeToChar(
                       PacBio::Data::CigarOperationType::REFERENCE_SKIP));
    EXPECT_EQ(
        'S', PacBio::Data::CigarOperation::TypeToChar(PacBio::Data::CigarOperationType::SOFT_CLIP));
    EXPECT_EQ(
        'H', PacBio::Data::CigarOperation::TypeToChar(PacBio::Data::CigarOperationType::HARD_CLIP));
    EXPECT_EQ('P',
              PacBio::Data::CigarOperation::TypeToChar(PacBio::Data::CigarOperationType::PADDING));
    EXPECT_EQ('=', PacBio::Data::CigarOperation::TypeToChar(
                       PacBio::Data::CigarOperationType::SEQUENCE_MATCH));
    EXPECT_EQ('X', PacBio::Data::CigarOperation::TypeToChar(
                       PacBio::Data::CigarOperationType::SEQUENCE_MISMATCH));
}

TEST(Data_CigarOperation, can_convert_char_to_type)
{
    EXPECT_EQ(PacBio::Data::CigarOperationType::ALIGNMENT_MATCH,
              PacBio::Data::CigarOperation::CharToType('M'));
    EXPECT_EQ(PacBio::Data::CigarOperationType::INSERTION,
              PacBio::Data::CigarOperation::CharToType('I'));
    EXPECT_EQ(PacBio::Data::CigarOperationType::DELETION,
              PacBio::Data::CigarOperation::CharToType('D'));
    EXPECT_EQ(PacBio::Data::CigarOperationType::REFERENCE_SKIP,
              PacBio::Data::CigarOperation::CharToType('N'));
    EXPECT_EQ(PacBio::Data::CigarOperationType::SOFT_CLIP,
              PacBio::Data::CigarOperation::CharToType('S'));
    EXPECT_EQ(PacBio::Data::CigarOperationType::HARD_CLIP,
              PacBio::Data::CigarOperation::CharToType('H'));
    EXPECT_EQ(PacBio::Data::CigarOperationType::PADDING,
              PacBio::Data::CigarOperation::CharToType('P'));
    EXPECT_EQ(PacBio::Data::CigarOperationType::SEQUENCE_MATCH,
              PacBio::Data::CigarOperation::CharToType('='));
    EXPECT_EQ(PacBio::Data::CigarOperationType::SEQUENCE_MISMATCH,
              PacBio::Data::CigarOperation::CharToType('X'));
}

TEST(Data_CigarOperation, can_set_via_type)
{
    PacBio::Data::CigarOperation c2;
    c2.Type(PacBio::Data::CigarOperationType::INSERTION);
    PacBio::Data::CigarOperation c3;
    c3.Type(PacBio::Data::CigarOperationType::DELETION);
    PacBio::Data::CigarOperation c4;
    c4.Type(PacBio::Data::CigarOperationType::REFERENCE_SKIP);
    PacBio::Data::CigarOperation c5;
    c5.Type(PacBio::Data::CigarOperationType::SOFT_CLIP);
    PacBio::Data::CigarOperation c6;
    c6.Type(PacBio::Data::CigarOperationType::HARD_CLIP);
    PacBio::Data::CigarOperation c7;
    c7.Type(PacBio::Data::CigarOperationType::PADDING);
    PacBio::Data::CigarOperation c8;
    c8.Type(PacBio::Data::CigarOperationType::SEQUENCE_MATCH);
    PacBio::Data::CigarOperation c9;
    c9.Type(PacBio::Data::CigarOperationType::SEQUENCE_MISMATCH);

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
    PacBio::Data::CigarOperation c2;
    c2.Char('I');
    PacBio::Data::CigarOperation c3;
    c3.Char('D');
    PacBio::Data::CigarOperation c4;
    c4.Char('N');
    PacBio::Data::CigarOperation c5;
    c5.Char('S');
    PacBio::Data::CigarOperation c6;
    c6.Char('H');
    PacBio::Data::CigarOperation c7;
    c7.Char('P');
    PacBio::Data::CigarOperation c8;
    c8.Char('=');
    PacBio::Data::CigarOperation c9;
    c9.Char('X');

    EXPECT_EQ(PacBio::Data::CigarOperationType::INSERTION, c2.Type());
    EXPECT_EQ(PacBio::Data::CigarOperationType::DELETION, c3.Type());
    EXPECT_EQ(PacBio::Data::CigarOperationType::REFERENCE_SKIP, c4.Type());
    EXPECT_EQ(PacBio::Data::CigarOperationType::SOFT_CLIP, c5.Type());
    EXPECT_EQ(PacBio::Data::CigarOperationType::HARD_CLIP, c6.Type());
    EXPECT_EQ(PacBio::Data::CigarOperationType::PADDING, c7.Type());
    EXPECT_EQ(PacBio::Data::CigarOperationType::SEQUENCE_MATCH, c8.Type());
    EXPECT_EQ(PacBio::Data::CigarOperationType::SEQUENCE_MISMATCH, c9.Type());
}

TEST(Data_CigarOperation, throws_on_invalid_op)
{
#ifdef PBCOPPER_PERMISSIVE_CIGAR

    // ctor(c, len)
    EXPECT_NO_THROW(PacBio::Data::CigarOperation('M', 1));

    // ctor(enum, len)
    EXPECT_NO_THROW(
        PacBio::Data::CigarOperation(PacBio::Data::CigarOperationType::ALIGNMENT_MATCH, 1));

    // setter via enum
    EXPECT_NO_THROW({
        PacBio::Data::CigarOperation c;
        c.Type(PacBio::Data::CigarOperationType::ALIGNMENT_MATCH);
    });

    // setter via char code
    EXPECT_NO_THROW({
        PacBio::Data::CigarOperation c;
        c.Char('M');
    });

#else

    // ctor(c, len)
    EXPECT_THROW({ PacBio::Data::CigarOperation('M', 1); }, std::runtime_error);

    // ctor(enum, len)
    EXPECT_THROW(
        { PacBio::Data::CigarOperation(PacBio::Data::CigarOperationType::ALIGNMENT_MATCH, 1); },
        std::runtime_error);

    // setter via enum
    EXPECT_THROW(
        {
            PacBio::Data::CigarOperation c;
            c.Type(PacBio::Data::CigarOperationType::ALIGNMENT_MATCH);
        },
        std::runtime_error);

    // setter via char code
    EXPECT_THROW(
        {
            PacBio::Data::CigarOperation c;
            c.Char('M');
        },
        std::runtime_error);

#endif
}

TEST(Data_CigarOperation, input_operator)
{
    const std::string s{"42="};
    std::istringstream in{s};

    PacBio::Data::CigarOperation op;
    in >> op;

    EXPECT_EQ(42, op.Length());
    EXPECT_EQ('=', op.Char());
    EXPECT_EQ(PacBio::Data::CigarOperationType::SEQUENCE_MATCH, op.Type());
}

TEST(Data_CigarOperation, output_operator)
{
    const PacBio::Data::CigarOperation op{'=', 42};

    std::ostringstream out;
    out << op;

    EXPECT_EQ("42=", out.str());
}
