// Copyright (c) 2016, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Armin Töpfer

#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <pbcopper/parallel/WorkQueue.h>

using PacBio::Parallel::WorkQueue;

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
