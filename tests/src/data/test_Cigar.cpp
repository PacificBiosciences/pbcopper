// Author: Derek Barnett
#include <pbcopper/data/Cigar.h>

#include <string>

#include <gtest/gtest.h>

TEST(Data_Cigar, default_is_empty)
{
    const PacBio::Data::Cigar cigar;
    EXPECT_TRUE(cigar.empty());
}

TEST(Data_Cigar, cigar_from_empty_string)
{
    const std::string emptyCigar = "";

    const PacBio::Data::Cigar cigar{emptyCigar};
    EXPECT_TRUE(cigar.empty());
}

TEST(Data_Cigar, cigar_from_single_op_string)
{
    const std::string singleCigar = "100=";

    const PacBio::Data::Cigar cigar{singleCigar};
    ASSERT_TRUE(cigar.size() == 1);

    const PacBio::Data::CigarOperation& op = cigar.front();
    EXPECT_TRUE(op.Char() == '=');
    EXPECT_TRUE(op.Length() == 100);
}

TEST(Data_Cigar, cigar_from_multi_op_string)
{
    const std::string multiCigar = "100=2D34I6=6X6=";

    const PacBio::Data::Cigar cigar{multiCigar};
    ASSERT_TRUE(cigar.size() == 6);

    PacBio::Data::CigarOperation op0 = cigar.at(0);
    PacBio::Data::CigarOperation op1 = cigar.at(1);
    PacBio::Data::CigarOperation op2 = cigar.at(2);
    PacBio::Data::CigarOperation op3 = cigar.at(3);
    PacBio::Data::CigarOperation op4 = cigar.at(4);
    PacBio::Data::CigarOperation op5 = cigar.at(5);

    EXPECT_TRUE(op0.Char() == '=');
    EXPECT_TRUE(op0.Length() == 100);
    EXPECT_TRUE(op1.Char() == 'D');
    EXPECT_TRUE(op1.Length() == 2);
    EXPECT_TRUE(op2.Char() == 'I');
    EXPECT_TRUE(op2.Length() == 34);
    EXPECT_TRUE(op3.Char() == '=');
    EXPECT_TRUE(op3.Length() == 6);
    EXPECT_TRUE(op4.Char() == 'X');
    EXPECT_TRUE(op4.Length() == 6);
    EXPECT_TRUE(op5.Char() == '=');
    EXPECT_TRUE(op5.Length() == 6);
}

TEST(Data_Cigar, empty_cigar_to_string)
{
    const std::string empty;
    PacBio::Data::Cigar cigar;
    EXPECT_EQ(empty, cigar.ToStdString());
}

TEST(Data_Cigar, single_op_cigar_to_string)
{
    const std::string singleCigar = "100=";

    PacBio::Data::Cigar cigar;
    cigar.emplace_back(PacBio::Data::CigarOperationType::SEQUENCE_MATCH, 100);

    EXPECT_EQ(singleCigar, cigar.ToStdString());
}

TEST(Data_Cigar, multi_op_cigar_to_string)
{
    const std::string multiCigar = "100=2D34I6=6X6=";

    PacBio::Data::Cigar cigar;
    cigar.emplace_back(PacBio::Data::CigarOperationType::SEQUENCE_MATCH, 100);
    cigar.emplace_back(PacBio::Data::CigarOperationType::DELETION, 2);
    cigar.emplace_back(PacBio::Data::CigarOperationType::INSERTION, 34);
    cigar.emplace_back(PacBio::Data::CigarOperationType::SEQUENCE_MATCH, 6);
    cigar.emplace_back(PacBio::Data::CigarOperationType::SEQUENCE_MISMATCH, 6);
    cigar.emplace_back(PacBio::Data::CigarOperationType::SEQUENCE_MATCH, 6);

    EXPECT_EQ(multiCigar, cigar.ToStdString());
}

TEST(Data_Cigar, input_operator)
{
    const std::string s{"100=2D34I6=6X6="};
    std::istringstream in{s};

    PacBio::Data::Cigar cigar;
    in >> cigar;

    EXPECT_EQ(s, cigar.ToStdString());
}

TEST(Data_Cigar, output_operator)
{
    const std::string s{"100=2D34I6=6X6="};
    const PacBio::Data::Cigar cigar{s};

    std::ostringstream out;
    out << cigar;

    EXPECT_EQ(s, out.str());
}
