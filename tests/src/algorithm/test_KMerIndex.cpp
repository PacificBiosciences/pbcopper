#include <pbcopper/algorithm/KMerIndex.h>
#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/utility/Random.h>
#include <pbcopper/utility/Ssize.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <random>

using namespace PacBio;
using namespace Algorithm;

// clang-format off

template<typename IndexType>
void TestKMerIndex(int bottomKValue=-1) {
    static constexpr int NUMBER_OF_FOUNDERS = 2500;
    constexpr int K = 31;
    std::vector<uint64_t> kmerFounders(NUMBER_OF_FOUNDERS);
    uint64_t seed = 0;
    const uint64_t kmerMask = uint64_t(-1) >> ((32 - K) * 2);
    for(uint64_t& x: kmerFounders) {
        x = Utility::WyHash64Step(seed) & kmerMask;
    }
    const std::vector<SubMerSelection> submerSelection{SubMerSelection(K, 0x3fffffff)};
    Algorithm::KMerIndex index(K,  submerSelection, bottomKValue);
    index.Insert(kmerFounders.begin(), kmerFounders.end());
    EXPECT_EQ(index.IsSorted(), true);
    {
        Algorithm::KMerIndex index2(K, submerSelection, bottomKValue);
        index2.Insert(kmerFounders);
        EXPECT_EQ(index, index2);
    }
    constexpr int NUM_CHOICES = 128;
    for(int i = 0; i < NUM_CHOICES; ++i) {
        const int pos = Utility::WyHash64Step(seed) % kmerFounders.size();
        uint64_t kmer = kmerFounders[pos];
        auto ret = index.Query(kmer);
        EXPECT_EQ(static_cast<int>(ret.front().first), pos);
    }
    for(const int numThreads: {8, 16}) {
        Algorithm::KMerIndex parallelIndex(K, submerSelection, bottomKValue);
        parallelIndex.InsertParallel(kmerFounders.begin(), kmerFounders.end(), numThreads);
        parallelIndex.Sort();
        EXPECT_EQ(parallelIndex.IsSorted(), true);
        EXPECT_EQ(parallelIndex, index);
    }
}

template<typename IndexType>
void TestFlexibleIndex(int bottomKValue=-1) {
    static constexpr int NUMBER_OF_FOUNDERS = 2500;
    constexpr int K = 31;
    constexpr int SUBK = 17;
    constexpr int NUM_SPACINGS = 32;
    using Algorithm::SubMerSelection;
    std::vector<uint64_t> kmerFounders(NUMBER_OF_FOUNDERS);
    uint64_t seed = 0;
    const uint64_t kmerMask = uint64_t(-1) >> ((32 - K) * 2);
    for(uint64_t& x: kmerFounders) {
        x = Utility::WyHash64Step(seed) & kmerMask;
    }
    std::vector<SubMerSelection> submerSelection{Algorithm::FlatSubMer(K, SUBK)};
    {
        const auto submers = Algorithm::GenerateRandomSubsequences(K, SUBK, NUM_SPACINGS);
        submerSelection.insert(submerSelection.end(), submers.begin(), submers.end());
    }
    // Uniquify
    std::sort(submerSelection.begin(), submerSelection.end());
    submerSelection.erase(std::unique(submerSelection.begin(), submerSelection.end()), submerSelection.end());
    Algorithm::KMerIndex index(K,  submerSelection, bottomKValue);
    index.Insert(kmerFounders.begin(), kmerFounders.end());
    int numNotFound = 0;
    constexpr int NUM_CHOICES = 128;
    constexpr int MAX_NEIGHBORS = 10;
    for(int i = 0; i < NUM_CHOICES; ++i) {
        const int pos = Utility::WyHash64Step(seed) % kmerFounders.size();
        const uint64_t kmer = kmerFounders[pos];
        auto ret = index.Query(kmer);
        EXPECT_EQ(static_cast<int>(ret.front().first), pos);
        int nNeighborsQueried = 0;
        for(const auto neighbor: Pbmer::DnaBit(kmer, 0, K).Neighbors()) {
            auto nret = index.Query(neighbor.mer);
            if(nret.empty()) {
                ++numNotFound;
            } else if(nret.size() == 1) {
                EXPECT_EQ(static_cast<int>(nret.front().first), pos);
            } else {
                const bool inList = std::find_if(nret.begin(), nret.end(), [pos](auto x) {return int(x.first) == pos;}) != nret.end();
                EXPECT_EQ(inList, true);
            }
            if(++nNeighborsQueried == MAX_NEIGHBORS) {
                break;
            }
        }
    }
    EXPECT_EQ(numNotFound, 0);
}

TEST(Algorithm_kmer_index, kmer_index_construction_and_accuracy32_exact) {
    for(const int bk: {137, 731, -1}) {
        TestKMerIndex<Algorithm::KMerIndex>(bk);
        TestKMerIndex<Algorithm::KMerLSH32>(bk);
        TestKMerIndex<Algorithm::KMerLSH32_64>(bk);
    }
}

TEST(Algorithm_kmer_index, kmer_index_construction_and_accuracy64_exact) {
    for(const int bk: {137, 731, -1}) {
        TestKMerIndex<Algorithm::KMerIndex>(bk);
        TestKMerIndex<Algorithm::KMerLSH64>(bk);
        TestKMerIndex<Algorithm::KMerLSH64_32>(bk);
    }
}
TEST(Algorithm_kmer_index, kmer_index_construction_and_accuracy32_flexible) {
    for(const int bk: {137, 731, -1}) {
        TestFlexibleIndex<Algorithm::KMerIndex>(bk);
        TestFlexibleIndex<Algorithm::KMerLSH32>(bk);
        TestFlexibleIndex<Algorithm::KMerLSH32_64>(bk);
    }
}

TEST(Algorithm_kmer_index, kmer_index_construction_and_accuracy64_flexible) {
    for(const int bk: {137, 731, -1}) {
        TestFlexibleIndex<Algorithm::KMerIndex>(bk);
        TestFlexibleIndex<Algorithm::KMerLSH64>(bk);
        TestFlexibleIndex<Algorithm::KMerLSH64_64>(bk);
    }
}

template<typename Index>
void TestThrowKGreaterThan32()
{
    auto func = [&]() {
        Index index(73,  Algorithm::FlatSubMer(73, 31));
    };
    EXPECT_THROW(func(), std::invalid_argument);
}

TEST(Algorithm_kmer_index, kmer_index_throws_if_k_gt32) {
    TestThrowKGreaterThan32<Algorithm::KMerIndex>();
    TestThrowKGreaterThan32<Algorithm::KMerLSH64>();
    TestThrowKGreaterThan32<Algorithm::KMerLSH64_32>();
    TestThrowKGreaterThan32<Algorithm::KMerLSH32>();
    TestThrowKGreaterThan32<Algorithm::KMerLSH32_32>();
}
