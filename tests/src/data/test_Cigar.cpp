// Copyright (c) 2016, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

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

    const Cigar cigar{ emptyCigar };
    EXPECT_TRUE(cigar.empty());
}

TEST(Data_Cigar, cigar_from_single_op_string)
{
    const std::string singleCigar = "100=";

    const Cigar cigar{ singleCigar };
    ASSERT_TRUE(cigar.size() == 1);

    const CigarOperation& op = cigar.front();
    EXPECT_TRUE(op.Char()   == '=');
    EXPECT_TRUE(op.Length() == 100);
}

TEST(Data_Cigar, cigar_from_multi_op_string)
{
    const std::string multiCigar = "100=2D34I6=6X6=";

    const Cigar cigar{ multiCigar };
    ASSERT_TRUE(cigar.size() == 6);

    CigarOperation op0 = cigar.at(0);
    CigarOperation op1 = cigar.at(1);
    CigarOperation op2 = cigar.at(2);
    CigarOperation op3 = cigar.at(3);
    CigarOperation op4 = cigar.at(4);
    CigarOperation op5 = cigar.at(5);

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
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH,  100);
    cigar.emplace_back(CigarOperationType::DELETION,          2);
    cigar.emplace_back(CigarOperationType::INSERTION,        34);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH,    6);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MISMATCH, 6);
    cigar.emplace_back(CigarOperationType::SEQUENCE_MATCH,    6);

    EXPECT_EQ(multiCigar, cigar.ToStdString());
}

TEST(Data_Cigar, input_operator)
{
    const std::string s = "100=2D34I6=6X6=";
    std::stringstream in(s);

    Cigar cigar;
    in >> cigar;

    EXPECT_EQ(s, cigar.ToStdString());
}

TEST(Data_Cigar, output_operator)
{
    const std::string s = "100=2D34I6=6X6=";
    const Cigar cigar { s };

    std::stringstream out;
    out << cigar;

    EXPECT_EQ(s, out.str());
}
