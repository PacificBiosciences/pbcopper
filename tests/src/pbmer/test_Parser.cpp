
#include <iostream>

#include <gtest/gtest.h>

#include <pbcopper/pbmer/Mers.h>
#include <pbcopper/pbmer/Parser.h>

TEST(Pbmer_Parser, parser_throws_if_dna_shorter_than_kmer)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGT"};
    EXPECT_THROW(parser.Parse(td1), std::runtime_error);
}

TEST(Pbmer_Parser, test_stub_for_parsing_normal_dna)
{
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};

    const PacBio::Pbmer::Parser parser{16};
    const PacBio::Pbmer::Mers m{parser.Parse(td1)};

    EXPECT_EQ(m.forward.size(), m.reverse.size());
    EXPECT_EQ(0, m.minimizers.size());
    // N-1 kmers
    EXPECT_EQ(29, m.forward.size());  // size(dna) - kmerSize + 1 ; 44 - 16 + 1
    EXPECT_EQ(408389973, m.forward.front().mer);
    EXPECT_EQ(2862426843, m.reverse.front().mer);
    EXPECT_EQ(1, m.forward.front().pos);
    //The complement position is on the foward strand.
    EXPECT_EQ(1, m.reverse.front().pos);
}
