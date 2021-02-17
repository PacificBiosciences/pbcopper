#pragma once

#include <string>
#include <vector>

// code from: https://github.com/yechengxi/Sparc

/*
MIT License

Copyright (c) 2018 Chengxi Ye

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

namespace Sparc {

struct Query
{
    std::string qAlignedSeq;
    std::string tAlignedSeq;
    int tStart;
    int tEnd;
};

std::string SparcConsensus(const std::string& backbone, std::vector<Query>& queries, int32_t gap,
                           size_t k, int32_t MinCov, double threshold);
std::string SparcSimpleConsensus(const std::string& backbone, std::vector<Query>& queries,
                                 int32_t MinCov, double threshold);
}  // namespace Sparc
