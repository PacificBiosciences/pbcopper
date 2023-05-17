#include <pbcopper/parallel/FireAndForgetIndexed.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

TEST(Parallel_FireAndForgetIndexed, strings)
{
    static const std::size_t numThreads = 3;
    static const std::size_t numElements = 10000;
    std::size_t initial = 0;
    std::size_t extra = 0;
    std::vector<std::size_t> vec;
    auto finish = [&](std::size_t index) { ++vec[index]; };
    PacBio::Parallel::FireAndForgetIndexed faf{numThreads, 1, finish};

    for (std::size_t i = 0; i < numThreads; ++i) {
        vec.emplace_back(i);
        initial += i;
    }
    ASSERT_EQ(vec.size(), numThreads);

    auto Submit = [&vec](std::size_t index, std::size_t data) { vec[index] += data; };

    for (std::size_t data = 0; data < numElements; ++data) {
        EXPECT_NO_THROW(faf.ProduceWith(Submit, data));
        extra += data;
    }

    EXPECT_NO_THROW(faf.Finalize());

    std::size_t expected = initial + extra + numThreads;
    std::size_t observed = 0;
    for (auto& v : vec) {
        observed += v;
    }

    // We do not know the order, but all the work is done.
    EXPECT_EQ(expected, observed);
}

TEST(Parallel_FireAndForgetIndexed, exceptionFinalize)
{
    static const std::size_t numThreads = 3;
    std::vector<std::size_t> vec(numThreads);
    auto finish = [&](std::size_t index) { ++vec[index]; };
    PacBio::Parallel::FireAndForgetIndexed faf{numThreads, 1, finish};

    auto SubmitSleep = [&vec](std::size_t index, std::size_t data) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        vec[index] += data;
    };
    auto SubmitExc = [&vec](std::size_t index, std::size_t data) {
        throw std::runtime_error{"fafi abort"};
        vec[index] += data;
    };
    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep, 1));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep, 1));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitExc, 1));

    EXPECT_ANY_THROW(faf.Finalize());

    std::size_t observed = 0;
    for (auto& v : vec) {
        observed += v;
    }

    EXPECT_EQ(observed, 2);
}

TEST(Parallel_FireAndForgetIndexed, exceptionProduceWith)
{
    static const std::size_t numThreads = 3;
    std::vector<std::size_t> vec(numThreads);
    auto finish = [&](std::size_t index) { ++vec[index]; };
    PacBio::Parallel::FireAndForgetIndexed faf{numThreads, 1, finish};

    auto SubmitSleep = [&vec](std::size_t index, std::size_t data) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        vec[index] += data;
    };
    auto SubmitExc = [&vec](std::size_t index, std::size_t data) {
        throw std::runtime_error{"fafi abort"};
        vec[index] += data;
    };
    auto Submit = [&vec](std::size_t index, std::size_t data) { vec[index] += data; };
    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep, 1));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitSleep, 1));
    EXPECT_NO_THROW(faf.ProduceWith(SubmitExc, 1));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_ANY_THROW(faf.ProduceWith(Submit, 1););
    EXPECT_ANY_THROW(faf.Finalize());

    std::size_t observed = 0;
    for (auto& v : vec) {
        observed += v;
    }

    EXPECT_EQ(observed, 2);
}

TEST(Parallel_FireAndForgetIndexed, exceptionFinish)
{
    static const std::size_t numThreads = 3;
    std::vector<std::size_t> vec(numThreads);
    auto finish = [&](std::size_t index) {
        throw std::runtime_error{"finish abort"};
        ++vec[index];
    };
    PacBio::Parallel::FireAndForgetIndexed faf{numThreads, 1, finish};

    auto Submit = [&vec](std::size_t index, std::size_t data) { vec[index] += data; };
    EXPECT_NO_THROW(faf.ProduceWith(Submit, 1));
    EXPECT_NO_THROW(faf.ProduceWith(Submit, 1));
    EXPECT_NO_THROW(faf.ProduceWith(Submit, 1));

    EXPECT_ANY_THROW(faf.Finalize());

    std::size_t observed = 0;
    for (auto& v : vec) {
        observed += v;
    }

    EXPECT_EQ(observed, 3);
}
