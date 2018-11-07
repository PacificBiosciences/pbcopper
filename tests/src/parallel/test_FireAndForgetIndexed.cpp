// Author: Armin Töpfer

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
#include <pbcopper/parallel/FireAndForgetIndexed.h>

using PacBio::Parallel::FireAndForgetIndexed;

TEST(Parallel_FireAndForgetIndexed, strings)
{
    static const size_t numThreads = 3;
    static const size_t numElements = 10000;
    PacBio::Parallel::FireAndForgetIndexed faf(numThreads);

    std::vector<size_t> vec;

    size_t initial = 0;
    for (size_t i = 0; i < numThreads; ++i) {
        vec.emplace_back(i);
        initial += i;
    }
    ASSERT_EQ(vec.size(), numThreads);

    std::atomic_int waiting{0};

    auto Submit = [&vec, &waiting](size_t index, size_t data) {
        vec[index] += data;
        --waiting;
    };

    size_t extra = 0;
    for (size_t data = 0; data < numElements; ++data) {
        ++waiting;
        faf.ProduceWith(Submit, data);
        extra += data;
    }

    faf.Finalize();
    while (waiting) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    size_t expected = initial + extra;
    size_t observed = 0;
    for (auto& v : vec)
        observed += v;

    // We do not know the order, but all the work is done.
    EXPECT_EQ(expected, observed);
}
