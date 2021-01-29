#include <pbcopper/utility/Shuffle.h>

#include <vector>

#include <gtest/gtest.h>

using namespace PacBio;

TEST(Utility_Shuffle, can_stably_shuffle_container)
{
    const std::vector<int> expected{5, 1, 3, 6, 7, 8, 0, 9, 2, 4};

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Utility::Shuffle(v, 42);
    EXPECT_EQ(v, expected);
}

TEST(Utility_Shuffle, can_stably_shuffle_range)
{
    // clang-format off
    const std::vector<int> expected{
        2, 0, 3, 1, 4,   // shuffled
        5, 6, 7, 8, 9};  // untouched
    // clang-format on

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Utility::Shuffle(v.begin(), v.begin() + 5, 42);
    EXPECT_EQ(v, expected);
}

TEST(Utility_Shuffle, can_stably_shuffle_container_reusing_mte)
{
    const std::vector<int> expected{5, 1, 3, 6, 7, 8, 0, 9, 2, 4};

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::mt19937_64 eng;
    Utility::Shuffle(v, eng, 42);
    EXPECT_EQ(v, expected);
}

TEST(Utility_Shuffle, can_stably_shuffle_range_reusing_mte)
{
    // clang-format off
    const std::vector<int> expected{
        2, 0, 3, 1, 4,   // shuffled
        5, 6, 7, 8, 9};  // untouched
    // clang-format on

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::mt19937_64 eng;
    Utility::Shuffle(v.begin(), v.begin() + 5, eng, 42);
    EXPECT_EQ(v, expected);
}
