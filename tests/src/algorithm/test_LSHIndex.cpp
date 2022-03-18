#include <pbcopper/algorithm/KMerIndex.h>
#include <pbcopper/algorithm/LSHIndex.h>
#include <pbcopper/utility/Random.h>
#include <pbcopper/utility/Ssize.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <random>

using namespace PacBio;

// clang-format off

TEST(Algorithm_lsh_index, lsh_index_test_accuracy) {
    static constexpr int NF = 100;
    static constexpr int NL = 200;
    std::vector<uint64_t> kmerFounders(NF);
    uint64_t seed = 13;
    auto sample = [&]() {return Utility::WyHash64Step(seed);};
    for(auto& x: kmerFounders) {
        x = sample() >> 4;
    }
    const int bk = 16;
    std::vector<std::vector<uint64_t>> kmerSets(NL, kmerFounders);
    for(int i = 0; i < NL; ++i) {
        auto& ks =kmerSets[i];
        for(int j = 0; j < i + 5; ++j) {
            ks[sample() % ks.size()] = sample();
        }
        std::transform(ks.begin(), ks.end(), ks.begin(), Algorithm::SubMerSelection::WangHash);
        std::sort(ks.begin(), ks.end());
        ks.resize(bk);
    }
    // Test that building the index twice yields the same index
    Algorithm::LSHIndex<uint64_t, uint32_t> bkidx = Algorithm::LSHIndex<uint64_t, uint32_t>::CreateBottomK(bk);
    bkidx.Insert(kmerSets.begin(), kmerSets.end());
    {
        Algorithm::LSHIndex<uint64_t, uint32_t> bkidx2;
        bkidx2.Insert(kmerSets.begin(), kmerSets.end());
        EXPECT_EQ(bkidx, bkidx2);
    }
    // Test that each item is its own best hit.
    for(int i = 0; i < NL; ++i) {
        const auto [ids, counts, numberNonZero] = bkidx.Query(kmerSets[i]);
        const auto findMyself = std::find_if(ids.begin(), ids.end(), [i](int x) {return x == i;});
        EXPECT_NE(findMyself, ids.end());
        const auto countit = counts.begin() + (findMyself - ids.begin());
        EXPECT_EQ(*countit, *std::max_element(counts.begin(), counts.end()));
    }
}

// clang-format on
