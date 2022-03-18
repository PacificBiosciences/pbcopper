#include <pbcopper/container/Unordered.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace PacBio;

TEST(Container_Unordered, InsertionInvariance)
{
    Container::UnorderedSet<int> rightHand;
    Container::UnorderedSet<int> leftHand;
    for (int i = 0; i < 10000; ++i) {
        rightHand.insert(10000 - i - 1);
        leftHand.insert(i);
    }
    EXPECT_EQ(rightHand, leftHand);
}

TEST(Container_Unordered, Counting)
{
    Container::UnorderedMap<int, int> rhs;
    std::unordered_map<int, int> lhs;
    for (int i = 0; i < 1000000; ++i) {
        rhs[i] = 1;
        lhs[i] = 1;
    }
    EXPECT_EQ(std::all_of(rhs.begin(), rhs.end(), [](const auto x) { return x.second == 1; }),
              true);
    for (const auto pair : rhs) {
        EXPECT_EQ(pair.second, lhs[pair.first]);
    }
}
