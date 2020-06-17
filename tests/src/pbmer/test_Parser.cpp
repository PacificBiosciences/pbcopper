#include <pbcopper/pbmer/Parser.h>

#include <gtest/gtest.h>

#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/pbmer/Mers.h>

TEST(Pbmer_Parser, parser_throws_if_dna_shorter_than_kmer)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGT"};
    EXPECT_THROW(parser.Parse(td1), std::runtime_error);
}

TEST(Pbmer_Parser, test_stub_for_parsing_normal_dna)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

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

TEST(Pbmer_Parser, compare_dnabit_parser)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers mers{parser.Parse(td1)};
    std::vector<PacBio::Pbmer::DnaBit> dnaBits = parser.ParseDnaBit(td1);
    for (size_t i = 0; i < mers.forward.size(); ++i) {
        EXPECT_EQ(dnaBits[i].mer, mers.forward[i].mer);
    }
}

TEST(Pbmer_Parser, test_RLE_size_a)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{""};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "");
}

TEST(Pbmer_Parser, test_RLE_size_b)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"A"};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "A");
}

TEST(Pbmer_Parser, test_RLE_a)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"AAATA"};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "ATA");
}

TEST(Pbmer_Parser, test_RLE_b)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"AAAATAA"};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "ATA");
}

TEST(Pbmer_Parser, test_RLE_c)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"GGAAAATAA"};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "GATA");
}

TEST(Pbmer_Parser, test_RLE_d)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"CATCAT"};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "CATCAT");
}

TEST(Pbmer_Parser, test_RLE_e)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"AAAAAA"};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "A");
}

TEST(Pbmer_Parser, test_RLE_f)
{
    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"CCCCCCC"};
    auto td2 = parser.RLE(td1);
    EXPECT_EQ(td2, "C");
}

TEST(Pbmer_Parser, test_RLE_destruct_size)
{
    const PacBio::Pbmer::Parser parser{16};
    std::string td1{""};
    parser.RLE(td1);
    EXPECT_EQ(td1, "");
}

TEST(Pbmer_Parser, test_RLE_destruct_a)
{
    const PacBio::Pbmer::Parser parser{16};
    std::string td1{"AAA"};
    parser.RLE(td1);
    EXPECT_EQ(td1, "A");
}

TEST(Pbmer_Parser, test_RLE_destruct_b)
{
    const PacBio::Pbmer::Parser parser{16};
    std::string td1{"AAAT"};
    parser.RLE(td1);
    EXPECT_EQ(td1, "AT");
}
