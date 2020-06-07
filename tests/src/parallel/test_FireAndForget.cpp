// Author: Armin Töpfer

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/parallel/FireAndForget.h>

TEST(Parallel_FireAndForget, strings)
{
    static const size_t numThreads = 3;
    static const size_t numElements = 10000;
    PacBio::Parallel::FireAndForget faf{numThreads};

    std::atomic_int waiting{0};
    auto Submit = [&waiting](std::string& input) {
        input = "done-" + input;
        --waiting;
    };

    std::vector<std::string> vec;

    for (size_t i = 0; i < numElements; ++i) {
        vec.emplace_back(std::to_string(i));
    }

    for (auto& v : vec) {
        ++waiting;
        EXPECT_NO_THROW(faf.ProduceWith(Submit, std::ref(v)));
    }

    EXPECT_NO_THROW(faf.Finalize());
    EXPECT_EQ(waiting, 0);

    for (auto& v : vec)
        EXPECT_EQ(v.substr(0, 4), "done");

    EXPECT_EQ(vec.size(), numElements);
}

TEST(Parallel_FireAndForget, exceptionFinalize)
{
    static const size_t numThreads = 3;
    PacBio::Parallel::FireAndForget faf{numThreads, 1};

    std::atomic_int counter{0};
    auto SubmitSleep = [&counter]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ++counter;
    };
    auto SubmitExc = [&counter]() {
        throw std::runtime_error("faf abort");
        ++counter;
    };

    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitExc));

    EXPECT_ANY_THROW(faf.Finalize());

    EXPECT_EQ(counter, 2);
}

TEST(Parallel_FireAndForget, exceptionProduceWith)
{
    static const size_t numThreads = 3;
    PacBio::Parallel::FireAndForget faf{numThreads, 1};

    std::atomic_int counter{0};
    auto SubmitSleep = [&counter]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++counter;
    };
    auto Submit = [&counter]() { ++counter; };
    auto SubmitExc = [&counter]() {
        throw std::runtime_error{"faf abort"};
        ++counter;
    };

    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitExc));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_ANY_THROW(faf.ProduceWith(Submit););
    EXPECT_ANY_THROW(faf.Finalize());

    EXPECT_EQ(counter, 2);
}
