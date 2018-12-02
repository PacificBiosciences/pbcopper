// Author: Armin Töpfer
#include <pbcopper/parallel/FireAndForgetIndexed.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

TEST(Parallel_FireAndForgetIndexed, strings)
{
    static const size_t numThreads = 3;
    static const size_t numElements = 10000;
    size_t initial = 0;
    size_t extra = 0;
    std::vector<size_t> vec;
    {
        PacBio::Parallel::FireAndForgetIndexed faf(numThreads);

        for (size_t i = 0; i < numThreads; ++i) {
            vec.emplace_back(i);
            initial += i;
        }
        ASSERT_EQ(vec.size(), numThreads);

        auto Submit = [&vec](size_t index, size_t data) { vec[index] += data; };

        for (size_t data = 0; data < numElements; ++data) {
            faf.ProduceWith(Submit, data);
            extra += data;
        }

        faf.Finalize();
    }

    size_t expected = initial + extra;
    size_t observed = 0;
    for (auto& v : vec)
        observed += v;

    // We do not know the order, but all the work is done.
    EXPECT_EQ(expected, observed);
}
