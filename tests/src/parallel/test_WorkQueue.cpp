// Author: Armin Töpfer
#include <pbcopper/parallel/WorkQueue.h>

#include <string>
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

    workQueue.Finalize();
    workerThread.wait();

    EXPECT_EQ(expected.size(), numElements);
    EXPECT_EQ(output.size(), numElements);

    for (size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(expected.at(i), output.at(i));
}
