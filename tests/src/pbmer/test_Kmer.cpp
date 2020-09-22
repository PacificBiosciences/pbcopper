#include <pbcopper/pbmer/Kmer.h>

#include <gtest/gtest.h>

TEST(Pbmer_Kmer, eq_check_overloaded_eqto_operation)
{
    PacBio::Pbmer::Kmer k1{PacBio::Data::Strand::FORWARD};
    const PacBio::Pbmer::Kmer k2{PacBio::Data::Strand::FORWARD};
    EXPECT_EQ(k1, k2);

    k1.mer = 2;
    EXPECT_NE(k1, k2);
}

//ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT
//ACGACCCTGAGCCCCC            TTTTTAAGAGAGGAGA
//TGCTGGGACTCGGGGG

TEST(Pbmer_Kmer, printKmer_check_bin_to_dna_for)
{
    const PacBio::Pbmer::Kmer k1{408389973, 1, PacBio::Data::Strand::FORWARD};
    const std::string got{k1.KmerToStr(16)};
    const std::string exp{"ACGACCCTGAGCCCCC"};
    EXPECT_EQ(exp, got);
}

TEST(Pbmer_Kmer, printKmer_check_bin_to_dna_rev)
{
    const PacBio::Pbmer::Kmer k1{2862426843, 1, PacBio::Data::Strand::REVERSE};
    const std::string got{k1.KmerToStr(16)};
    const std::string exp{"GGGGGCTCAGGGTCGT"};
    EXPECT_EQ(exp, got);
}
