// Author: Armin Töpfer

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <pbcopper/parallel/FireAndForget.h>

using PacBio::Parallel::FireAndForget;

TEST(Parallel_FireAndForget, strings)
{
    static const size_t numThreads = 3;
    static const size_t numElements = 10000;
    PacBio::Parallel::FireAndForget faf(numThreads);

    auto Submit = [](std::string& input) { input = "done-" + input; };

    std::vector<std::string> vec;

    for (size_t i = 0; i < numElements; ++i) {
        vec.emplace_back(std::to_string(i));
    }

    for (auto& v : vec)
        faf.ProduceWith(Submit, std::ref(v));

    faf.Finalize();

    for (auto& v : vec)
        EXPECT_EQ(v.substr(0, 4), "done");

    EXPECT_EQ(vec.size(), numElements);
}
