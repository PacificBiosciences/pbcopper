#include <pbcopper/poa/SparsePoa.h>

#include <cstddef>

#include <random>

#include <boost/assign.hpp>
#include <boost/assign/std/vector.hpp>

#include <gtest/gtest.h>

#include <pbcopper/poa/PoaConsensus.h>

using namespace boost::assign;

using namespace PacBio::Poa;

TEST(SparsePoaTest, TestLocalStaggered)
{
    // Adapted from Pat's C# test
    std::vector<std::string> reads;

    // Don't let clang-format break the whitespace here!!
    // clang-format off

    //        0123456789012345678901234567890
    reads += "TTTACAGGATAGTGCCGCCAATCTTCCAGT",
    //               0123456789012345678901234567890123456789012345
                    "GATACCCCGTGCCGCCAATCTTCCAGTATATACAGCACGAGTAGC",
    //                012345678901234567890123456789012345678901234567890123456789012345678
                     "ATAGTGCCGCCAATCTTCCAGTATATACAGCACGGAGTAGCATCACGTACGTACGTCTACACGTAATT",
    //                                                                    0123456789012345678901234567890123456
                                                                         "ACGTCTACACGTAATTTTGGAGAGCCCTCTCTCACG",
    //                                                                          01234567890123456789012345678901
                                                                               "ACACGTAATTTTGGAGAGCCCTCTCTTCACG",
    //             01234567890123456789012345678901234567890123456789012345
                  "AGGATAGTGCCGCCAATCTTCCAGTAATATACAGCACGGAGTAGCATCACGTACG",
    //                01234567890123456789012345678901234567890123456789012345678901234
                     "ATAGTGCCGCCAATCTTCCAGTATATACAGCACGGAGTAGCATCACGTACGTACGTCTACACGT";
    // -----------------------------------------------------------------------------------
    //                012345678901234567890123456789012345678901234567890123456789012345678
    //               "ATAGTGCCGCCAATCTTCCAGTATATACAGCACGGAGTAGCATCACGTACGTACGTCTACACGTAATT",

    // clang-format on

    SparsePoa sp;
    for (auto& read : reads) {
        SparsePoa::ReadKey id = sp.OrientAndAddRead(read);
        EXPECT_GE(id, 0);
    }

    std::vector<PoaAlignmentSummary> summaries;
    std::string consensusSeq = sp.FindConsensus(4, &summaries)->Sequence;

    EXPECT_EQ("ATAGTGCCGCCAATCTTCCAGTATATACAGCACGGAGTAGCATCACGTACGTACGTCTACACGTAATT", consensusSeq);

    EXPECT_FALSE(summaries[0].ReverseComplementedRead);
    EXPECT_EQ(Interval(8, 30), summaries[0].ExtentOnRead);
    EXPECT_EQ(Interval(0, 22), summaries[0].ExtentOnConsensus);

    EXPECT_FALSE(summaries[1].ReverseComplementedRead);
    EXPECT_EQ(Interval(8, 45), summaries[1].ExtentOnRead);
    EXPECT_EQ(Interval(3, 41), summaries[1].ExtentOnConsensus);

    EXPECT_FALSE(summaries[2].ReverseComplementedRead);
    EXPECT_EQ(Interval(0, 68), summaries[2].ExtentOnRead);
    EXPECT_EQ(Interval(0, 68), summaries[2].ExtentOnConsensus);

    EXPECT_FALSE(summaries[3].ReverseComplementedRead);
    EXPECT_EQ(Interval(0, 16), summaries[3].ExtentOnRead);
    EXPECT_EQ(Interval(52, 68), summaries[3].ExtentOnConsensus);

    EXPECT_FALSE(summaries[4].ReverseComplementedRead);
    EXPECT_EQ(Interval(0, 10), summaries[4].ExtentOnRead);
    EXPECT_EQ(Interval(58, 68), summaries[4].ExtentOnConsensus);

    EXPECT_FALSE(summaries[5].ReverseComplementedRead);
    EXPECT_EQ(Interval(3, 55), summaries[5].ExtentOnRead);
    EXPECT_EQ(Interval(0, 51), summaries[5].ExtentOnConsensus);

    EXPECT_FALSE(summaries[6].ReverseComplementedRead);
    EXPECT_EQ(Interval(0, 64), summaries[6].ExtentOnRead);
    EXPECT_EQ(Interval(0, 64), summaries[6].ExtentOnConsensus);
}

TEST(SparsePoaTest, TestOrientation)
{
    std::vector<std::string> reads = {"AAAGATTACAGGG", "CCCTGTAATCTTT", "AAAGATTACAGGG"};
    SparsePoa sp;
    for (auto& read : reads) {
        SparsePoa::ReadKey id = sp.OrientAndAddRead(read);
        EXPECT_GE(id, 0);
    }

    std::vector<PoaAlignmentSummary> summaries;
    std::string consensusSeq = sp.FindConsensus(2, &summaries)->Sequence;

    EXPECT_EQ("AAAGATTACAGGG", consensusSeq);

    EXPECT_FALSE(summaries[0].ReverseComplementedRead);
    EXPECT_TRUE(summaries[1].ReverseComplementedRead);
    EXPECT_FALSE(summaries[2].ReverseComplementedRead);
}

std::string rc(const std::string& a)
{
    const size_t len = a.length();
    std::string b;

    b.reserve(len);

    for (size_t i = 0; i < len; ++i) {
        char c = a[len - 1 - i];
        switch (c) {
            case 'A':
                c = 'T';
                break;
            case 'C':
                c = 'G';
                break;
            case 'G':
                c = 'C';
                break;
            case 'T':
                c = 'A';
                break;
        }
        b.push_back(c);
    }

    return b;
}

#if EXTENSIVE_TESTING
constexpr size_t NUM_ITERATIONS = 100;
#else
constexpr size_t NUM_ITERATIONS = 10;
#endif

TEST(SparsePoaTest, SingleReadNTimes)
{
    std::mt19937 gen(42);
    std::uniform_int_distribution<size_t> d(2000, 20000);
    std::uniform_int_distribution<size_t> b(0, 3);

    const std::string bases = "ACGT";

    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        size_t len = 0;
        std::string seq;

        while (len < 300) {
            len = d(gen);
        }

        seq.reserve(len);

        for (size_t j = 0; j < len; ++j) {
            seq.push_back(bases[b(gen)]);
        }

        SparsePoa sp;
        SparsePoa::ReadKey id = sp.OrientAndAddRead(seq);

        std::vector<PoaAlignmentSummary> summaries;
        std::string poa = sp.FindConsensus(1, &summaries)->Sequence;

        EXPECT_EQ(seq, poa);
        EXPECT_EQ(Interval(0, len), summaries[id].ExtentOnRead);
        EXPECT_EQ(Interval(0, len), summaries[id].ExtentOnConsensus);
        EXPECT_FALSE(summaries[id].ReverseComplementedRead);
    }
}

TEST(SparsePoaTest, SingleAndHalfNTimes)
{
    std::mt19937 gen(42);
    std::uniform_int_distribution<size_t> d(1000, 5000);
    std::uniform_int_distribution<size_t> b(0, 3);

    const std::string bases = "ACGT";

    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        size_t len = 0;
        std::string seq1, seq2;

        while (len < 300) {
            len = d(gen);
        }

        seq1.reserve(len);
        seq2.reserve(len / 3);

        for (size_t j = 0; j < len; ++j) {
            seq1.push_back(bases[b(gen)]);
        }

        seq2 = rc(seq1).substr(0, len / 3);

        SparsePoa sp;
        SparsePoa::ReadKey id1 = sp.OrientAndAddRead(seq1);
        SparsePoa::ReadKey id2 = sp.OrientAndAddRead(seq2);

        std::vector<PoaAlignmentSummary> summaries;
        std::string poa = sp.FindConsensus(1, &summaries)->Sequence;

        EXPECT_EQ(seq1, poa);
        EXPECT_EQ(Interval(0, len), summaries[id1].ExtentOnRead);
        EXPECT_EQ(Interval(0, len), summaries[id1].ExtentOnConsensus);
        EXPECT_FALSE(summaries[id1].ReverseComplementedRead);
        EXPECT_EQ(Interval(0, len / 3), summaries[id2].ExtentOnRead);
        EXPECT_EQ(Interval(len - len / 3, len), summaries[id2].ExtentOnConsensus);
        EXPECT_TRUE(summaries[id2].ReverseComplementedRead);
    }
}
