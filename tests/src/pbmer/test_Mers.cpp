#include <pbcopper/pbmer/Mers.h>

#include <unordered_set>

#include <gtest/gtest.h>

#include <pbcopper/pbmer/Kmer.h>
#include <pbcopper/pbmer/Parser.h>

TEST(Pbmer_Mers, test_hash)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    /* A few useful examples

    std::cout << "Kmers - forward strand:\n";

    for (const auto& kmer : m.forward)
        std::cout << kmer.KmerToStr(16) << " k:" << kmer.mer << " pos:" << kmer.pos << '\n';

    std::cout << "\n\n";
    std::cout << "Kmers - reverse strand:\n";

    for (auto it = m.reverse.rbegin(); it != m.reverse.rend(); ++it)
        std::cout << it->KmerToStr(16) << " k:" << it->mer << " pos:" << it->pos << '\n';

    std::cout << "\n\n";
    std::cout
        << "Minimizers - not windowed - all Kmers hashed and minimized on strand on strand:\n";
    */

    m.HashKmers();

    /*
    for (const auto& min : m.minimizers)
        std::cout << min.KmerToStr(16) << " k:" << min.mer << " pos:" << min.pos << '\n';
    */

    EXPECT_EQ(1897634152, m.minimizers.front().mer);
}

TEST(Pbmer_Mers, test_minimizer_logic_f_short)
{

    const PacBio::Pbmer::Parser parser{6};
    const std::string td1{"ACGACCCTGAGC"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    //Michael Roberts et al 2014. figure 1.

    m.HashKmers();
    EXPECT_EQ(m.minimizers.size(), 7);
    m.minimizers.at(0).mer = 2;
    m.minimizers.at(1).mer = 3;
    m.minimizers.at(2).mer = 1;
    m.minimizers.at(3).mer = 0;
    m.minimizers.at(4).mer = 3;
    m.minimizers.at(5).mer = 4;
    m.minimizers.at(6).mer = 3;

    m.WindowMin(3);

    EXPECT_EQ(m.minimizers.size(), 4);
    EXPECT_EQ(m.minimizers.at(0).mer, 1);
    EXPECT_EQ(m.minimizers.at(1).mer, 0);
    EXPECT_EQ(m.minimizers.at(2).mer, 3);
    EXPECT_EQ(m.minimizers.at(3).mer, 3);
}

TEST(Pbmer_Mers, test_minimizer_logic_f_long)
{

    const PacBio::Pbmer::Parser parser{6};
    const std::string td1{"ACGACCCTGAGCACTAC"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    //Michael Roberts et al 2014. figure 1.

    m.HashKmers();
    EXPECT_EQ(m.minimizers.size(), 12);
    m.minimizers.at(0).mer = 4;
    m.minimizers.at(1).mer = 2;
    m.minimizers.at(2).mer = 6;
    m.minimizers.at(3).mer = 4;
    m.minimizers.at(4).mer = 7;
    m.minimizers.at(5).mer = 2;
    m.minimizers.at(6).mer = 8;
    m.minimizers.at(7).mer = 1;
    m.minimizers.at(8).mer = 4;
    m.minimizers.at(9).mer = 7;
    m.minimizers.at(10).mer = 5;
    m.minimizers.at(11).mer = 1;

    m.WindowMin(7);

    EXPECT_EQ(m.minimizers.size(), 4);
    EXPECT_EQ(m.minimizers.at(0).mer, 2);
    EXPECT_EQ(m.minimizers.at(1).mer, 2);
    EXPECT_EQ(m.minimizers.at(2).mer, 1);
    EXPECT_EQ(m.minimizers.at(3).mer, 1);
}

TEST(Pbmer_Mers, test_minimizer_logic_r_long)
{

    const PacBio::Pbmer::Parser parser{6};
    const std::string td1{"ACGACCCTGAGCACTAC"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    //Michael Roberts et al 2014. figure 1.

    m.HashKmers();
    EXPECT_EQ(m.minimizers.size(), 12);
    m.minimizers.at(0).mer = 1;
    m.minimizers.at(1).mer = 5;
    m.minimizers.at(2).mer = 7;
    m.minimizers.at(3).mer = 4;
    m.minimizers.at(4).mer = 1;
    m.minimizers.at(5).mer = 8;
    m.minimizers.at(6).mer = 2;
    m.minimizers.at(7).mer = 7;
    m.minimizers.at(8).mer = 4;
    m.minimizers.at(9).mer = 6;
    m.minimizers.at(10).mer = 2;
    m.minimizers.at(11).mer = 4;

    m.WindowMin(7);

    EXPECT_EQ(m.minimizers.size(), 4);
    EXPECT_EQ(m.minimizers.at(0).mer, 1);
    EXPECT_EQ(m.minimizers.at(1).mer, 1);
    EXPECT_EQ(m.minimizers.at(2).mer, 2);
    EXPECT_EQ(m.minimizers.at(3).mer, 2);
}

TEST(Pbmer_Mers, test_minimizer_logic_r_short)
{

    const PacBio::Pbmer::Parser parser{6};
    const std::string td1{"ACGACCCTGAGC"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    //Michael Roberts et al 2014. figure 1.

    m.HashKmers();
    EXPECT_EQ(m.minimizers.size(), 7);
    m.minimizers.at(0).mer = 3;
    m.minimizers.at(1).mer = 4;
    m.minimizers.at(2).mer = 3;
    m.minimizers.at(3).mer = 0;
    m.minimizers.at(4).mer = 1;
    m.minimizers.at(5).mer = 3;
    m.minimizers.at(6).mer = 2;

    m.WindowMin(3);

    EXPECT_EQ(m.minimizers.size(), 4);
    EXPECT_EQ(m.minimizers.at(0).mer, 3);
    EXPECT_EQ(m.minimizers.at(1).mer, 3);
    EXPECT_EQ(m.minimizers.at(2).mer, 0);
    EXPECT_EQ(m.minimizers.at(3).mer, 1);
}

TEST(Pbmer_Mers, test_minimizers_size)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCCTCT"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    m.HashKmers();
    m.WindowMin(1);

    EXPECT_EQ(m.minimizers.size(), m.forward.size());
}

TEST(Pbmer_Mers, test_minimizer_logic_window_too_long)
{

    const PacBio::Pbmer::Parser parser{6};
    const std::string td1{"ACGACCCTGAGC"};
    PacBio::Pbmer::Mers m{parser.Parse(td1)};

    m.HashKmers();
    EXPECT_EQ(m.minimizers.size(), 7);
    m.minimizers.at(0).mer = 2;
    m.minimizers.at(1).mer = 3;
    m.minimizers.at(2).mer = 1;
    m.minimizers.at(3).mer = 7;
    m.minimizers.at(4).mer = 3;
    m.minimizers.at(5).mer = 1;
    m.minimizers.at(6).mer = 3;

    // return global minima for a window bigger than minimizer list
    m.WindowMin(99);

    EXPECT_EQ(m.minimizers.size(), 2);
    EXPECT_EQ(m.minimizers.at(0).mer, 1);
    EXPECT_EQ(m.minimizers.at(1).mer, 1);
}

TEST(Pbmer_Mers, test_nmps_matching)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCAACGCTCTCT"};
    const std::string td2{"AGAGAGCGTTGAGAGAATTTTTTTAGATGACTCTGGGGGCTCAGGGTCGT"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    m1.HashKmers();
    m1.WindowMin(3);
    m1.WindowMin(2);

    m2.HashKmers();
    m2.WindowMin(3);
    m2.WindowMin(2);

    std::vector<PacBio::Pbmer::Kmer> nmps1 = m1.BuildNMPs();
    std::vector<PacBio::Pbmer::Kmer> nmps2 = m2.BuildNMPs();

    EXPECT_EQ(9, nmps1.size());
    EXPECT_EQ(9, nmps2.size());

    for (unsigned int i = 0; i < nmps1.size(); i++) {
        EXPECT_EQ(nmps1.at(i).mer, nmps2.at(nmps2.size() - i - 1).mer);
    }
}

// checking that no minimizer == nmp. It is possible, but more likely a problem
// during hashing or nmp generation
TEST(Pbmer_Mers, test_nmps_hashing)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCAACGCTCTCT"};
    const std::string td2{"AGAGAGCGTTGAGAGAATTTTTTTAGATGACTCTGGGGGCTCAGGGTCGT"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    m1.HashKmers();
    m1.WindowMin(3);
    m1.WindowMin(2);

    m2.HashKmers();
    m2.WindowMin(3);
    m2.WindowMin(2);

    std::unordered_set<uint64_t> mins;

    for (const auto& x : m1.minimizers) {
        mins.insert(x.mer);
    }
    for (const auto& x : m2.minimizers) {
        mins.insert(x.mer);
    }

    std::vector<PacBio::Pbmer::Kmer> nmps1 = m1.BuildNMPs();
    std::vector<PacBio::Pbmer::Kmer> nmps2 = m2.BuildNMPs();

    EXPECT_EQ(9, nmps1.size());
    EXPECT_EQ(9, nmps2.size());

    for (unsigned int i = 0; i < nmps1.size(); i++) {
        EXPECT_EQ(nmps1.at(i).mer, nmps2.at(nmps2.size() - i - 1).mer);
        EXPECT_EQ(mins.find(nmps1.at(i).mer), mins.end());
        EXPECT_EQ(mins.find(nmps2.at(i).mer), mins.end());
    }
}

TEST(Pbmer_Mers, test_minimizer_lexsmaller)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCAACGCTCTCT"};
    const std::string td2{"AGAGAGCGTTGAGAGAATTTTTTTAGATGACTCTGGGGGCTCAGGGTCGT"};

    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};
    m1.HashKmers();
    m2.HashKmers();

    for (unsigned int i = 0; i < m1.minimizers.size(); ++i) {
        EXPECT_EQ(m1.minimizers[i].mer, m2.minimizers[m2.minimizers.size() - i - 1].mer);
    }
}

TEST(Pbmer_Mers, test_minimizer_strands_k2)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCAACGCTCTCT"};
    const std::string td2{"AGAGAGCGTTGAGAGAATTTTTTTAGATGACTCTGGGGGCTCAGGGTCGT"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    m1.HashKmers();
    m1.WindowMin(2);

    m2.HashKmers();
    m2.WindowMin(2);

    EXPECT_EQ(m1.minimizers.size(), m2.minimizers.size());

    for (unsigned int i = 0; i < m1.minimizers.size(); ++i) {
        EXPECT_EQ(m1.minimizers[i].mer, m2.minimizers[m2.minimizers.size() - i - 1].mer);
    }
}

TEST(Pbmer_Mers, test_minimizer_strands_k1)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCAACGCTCTCT"};
    const std::string td2{"AGAGAGCGTTGAGAGAATTTTTTTAGATGACTCTGGGGGCTCAGGGTCGT"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    m1.HashKmers();
    m1.WindowMin(1);

    m2.HashKmers();
    m2.WindowMin(1);

    EXPECT_EQ(m1.minimizers.size(), m2.minimizers.size());

    for (unsigned int i = 0; i < m1.minimizers.size(); ++i) {
        EXPECT_EQ(m1.minimizers[i].mer, m2.minimizers[m2.minimizers.size() - i - 1].mer);
    }
}

TEST(Pbmer_Mers, test_minimizer_strands_k10)
{

    const PacBio::Pbmer::Parser parser{16};
    const std::string td1{"ACGACCCTGAGCCCCCAGAGTCATCTAAAAAAATTCTCTCAACGCTCTCT"};
    const std::string td2{"AGAGAGCGTTGAGAGAATTTTTTTAGATGACTCTGGGGGCTCAGGGTCGT"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    PacBio::Pbmer::Mers m2{parser.Parse(td2)};

    m1.HashKmers();
    m1.WindowMin(10);

    m2.HashKmers();
    m2.WindowMin(10);

    EXPECT_EQ(m1.minimizers.size(), m2.minimizers.size());

    for (unsigned int i = 0; i < m1.minimizers.size(); ++i) {
        EXPECT_EQ(m1.minimizers[i].mer, m2.minimizers[m2.minimizers.size() - i - 1].mer);
    }
}

TEST(Pbmer_Mers, test_palindom)
{
    const PacBio::Pbmer::Parser parser{6};
    const std::string td1{"TCAAACTTTTTGAAATTTCAAAAAGTTTGA"};
    PacBio::Pbmer::Mers m1{parser.Parse(td1)};
    m1.HashKmers();
    m1.WindowMin(3);
    EXPECT_GT(m1.minimizers.size(), 0);
}
