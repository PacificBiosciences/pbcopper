// Author: Armin Töpfer
#include <pbcopper/parallel/WorkQueue.h>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

void WorkerThread(PacBio::Parallel::WorkQueue<std::string>& queue, std::vector<std::string>* output)
{
    auto LambdaWorker = [&](std::string&& ps) { output->emplace_back(std::move(ps)); };

    while (queue.ConsumeWith(LambdaWorker)) {
    }
}

TEST(Parallel_WorkQueue, strings)
{
    static const size_t numThreads = 3;
    static const size_t numElements = 10000;
    PacBio::Parallel::WorkQueue<std::string> workQueue(numThreads);

    std::vector<std::string> output;
    output.reserve(numElements);
    std::future<void> workerThread =
        std::async(std::launch::async, WorkerThread, std::ref(workQueue), &output);

    auto Submit = [](std::string& input) {
        input += "-done";
        return input;
    };

    std::vector<std::string> expected;
    expected.reserve(numElements);
    for (size_t i = 0; i < numElements; ++i) {
        std::string tmp = std::to_string(i);
        expected.emplace_back(tmp + "-done");
        workQueue.ProduceWith(Submit, std::move(tmp));
    }

    EXPECT_NO_THROW(workQueue.Finalize());
    EXPECT_NO_THROW(workerThread.wait());

    EXPECT_EQ(expected.size(), numElements);
    EXPECT_EQ(output.size(), numElements);

    for (size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(expected.at(i), output.at(i));
}

void WorkerThreadException(PacBio::Parallel::WorkQueue<std::string>& queue,
                           std::vector<std::string>* output)
{
    auto LambdaWorker = [&](std::string&& ps) { output->emplace_back(std::move(ps)); };

    try {
        while (queue.ConsumeWith(LambdaWorker)) {
        }
    } catch (...) {
        EXPECT_TRUE(false);
    }
}

TEST(Parallel_WorkQueue, exceptionProduceWith)
{
    static const size_t numThreads = 3;
    PacBio::Parallel::WorkQueue<std::string> workQueue(numThreads, 1);
    std::vector<std::string> output;
    std::future<void> workerThread =
        std::async(std::launch::async, WorkerThreadException, std::ref(workQueue), &output);

    auto Submit = [](std::string& input) {
        input += "-done";
        return input;
    };

    auto SubmitExc = [](std::string& input) {
        input += "-done";
        throw std::runtime_error("faf abort");
        return input;
    };

    EXPECT_NO_THROW(workQueue.ProduceWith(Submit, std::string("a")));
    EXPECT_NO_THROW(workQueue.ProduceWith(Submit, std::string("b")));
    EXPECT_NO_THROW(workQueue.ProduceWith(Submit, std::string("c")));
    EXPECT_NO_THROW(workQueue.ProduceWith(SubmitExc, std::string("0")));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_ANY_THROW(workQueue.ProduceWith(Submit, std::string("1")));

    EXPECT_NO_THROW(workQueue.FinalizeWorkers());
    EXPECT_NO_THROW(workerThread.wait());
    EXPECT_ANY_THROW(workQueue.Finalize());
}

TEST(Parallel_WorkQueue, exceptionFinalize)
{
    static const size_t numThreads = 3;
    PacBio::Parallel::WorkQueue<std::string> workQueue(numThreads, 1);
    std::vector<std::string> output;
    std::future<void> workerThread =
        std::async(std::launch::async, WorkerThreadException, std::ref(workQueue), &output);

    auto Submit = [](std::string& input) {
        input += "-done";
        return input;
    };

    auto SubmitExc = [](std::string& input) {
        input += "-done";
        throw std::runtime_error("faf abort");
        return input;
    };

    EXPECT_NO_THROW(workQueue.ProduceWith(Submit, std::string("a")));
    EXPECT_NO_THROW(workQueue.ProduceWith(Submit, std::string("b")));
    EXPECT_NO_THROW(workQueue.ProduceWith(Submit, std::string("c")));
    EXPECT_NO_THROW(workQueue.ProduceWith(SubmitExc, std::string("0")));

    EXPECT_NO_THROW(workQueue.FinalizeWorkers());
    EXPECT_NO_THROW(workerThread.wait());
    EXPECT_ANY_THROW(workQueue.Finalize());
}
