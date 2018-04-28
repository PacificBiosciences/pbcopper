// Author: Derek Barnett

#include <gtest/gtest.h>
#include <pbcopper/data/Cigar.h>
#include <string>

using PacBio::Data::Cigar;
using PacBio::Data::CigarOperation;
using PacBio::Data::CigarOperationType;

TEST(Data_Cigar, default_is_empty)
{
    const Cigar cigar;
    EXPECT_TRUE(cigar.empty());
}

TEST(Data_Cigar, cigar_from_empty_string)
{
    const std::string emptyCigar = "";

    const Cigar cigar{emptyCigar};
    EXPECT_TRUE(cigar.empty());
}

TEST(Data_Cigar, cigar_from_single_op_string)
{
    const std::string singleCigar = "100=";

    const Cigar cigar{singleCigar};
    ASSERT_TRUE(cigar.size() == 1);

    const CigarOperation& op = cigar.front();
    EXPECT_TRUE(op.Char() == '=');
    EXPECT_TRUE(op.Length() == 100);
}

TEST(Data_Cigar, cigar_from_multi_op_string)
{
    const std::string multiCigar = "100=2D34I6=6X6=";

    const Cigar cigar{multiCigar};
    ASSERT_TRUE(cigar.size() == 6);

    CigarOperation op0 = cigar.at(0);
    CigarOperation op1 = cigar.at(1);
    CigarOperation op2 = cigar.at(2);
    CigarOperation op3 = cigar.at(3);
    CigarOperation op4 = cigar.at(4);
    CigarOperation op5 = cigar.at(5);

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
    Cigar cigar;
    EXPECT_EQ(empty, cigar.ToStdString());
}

TEST(Data_Cigar, single_op_cigar_to_string)
{
    const std::string singleCigar = "100=";

    Cigar cigar;
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH, 100);

    EXPECT_EQ(singleCigar, cigar.ToStdString());
}

TEST(Data_Cigar, multi_op_cigar_to_string)
{
    const std::string multiCigar = "100=2D34I6=6X6=";

    Cigar cigar;
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH, 100);
    cigar.emplace_back(CigarOperationType::DELETION, 2);
    cigar.emplace_back(CigarOperationType::INSERTION, 34);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH, 6);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MISMATCH, 6);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH, 6);

    EXPECT_EQ(multiCigar, cigar.ToStdString());
}

TEST(Data_Cigar, input_operator)
{
    const std::string s{"100=2D34I6=6X6="};
    std::istringstream in{s};

    Cigar cigar;
    in >> cigar;

    EXPECT_EQ(s, cigar.ToStdString());
}

TEST(Data_Cigar, output_operator)
{
    const std::string s{"100=2D34I6=6X6="};
    const Cigar cigar{s};

    std::ostringstream out;
    out << cigar;

    EXPECT_EQ(s, out.str());
}
