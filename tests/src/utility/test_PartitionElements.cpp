#include <pbcopper/utility/PartitionElements.h>

#include <tuple>
#include <vector>

#include <gtest/gtest.h>

using namespace PacBio;

// ------------------------
// PartitionElementCounts
// ------------------------

TEST(Utility_PartitionElementCounts, throws_on_negative_max_bucket_count)
{
    const int numElements = 10;
    const int maxBuckets = -1;
    EXPECT_THROW(Utility::PartitionElementCounts(numElements, maxBuckets,
                                                 Utility::PartitionBehavior::DISCARD_EMPTY),
                 std::runtime_error);
    EXPECT_THROW(Utility::PartitionElementCounts(numElements, maxBuckets,
                                                 Utility::PartitionBehavior::KEEP_EMPTY),
                 std::runtime_error);
}

TEST(Utility_PartitionElementCounts, throws_on_zero_max_bucket_count)
{
    const int numElements = 10;
    const int maxBuckets = 0;
    EXPECT_THROW(Utility::PartitionElementCounts(numElements, maxBuckets,
                                                 Utility::PartitionBehavior::DISCARD_EMPTY),
                 std::runtime_error);
    EXPECT_THROW(Utility::PartitionElementCounts(numElements, maxBuckets,
                                                 Utility::PartitionBehavior::KEEP_EMPTY),
                 std::runtime_error);
}

TEST(Utility_PartitionElementCounts, throws_on_negative_num_elements)
{
    const int numElements = -1;
    const int maxBuckets = 5;
    EXPECT_THROW(Utility::PartitionElementCounts(numElements, maxBuckets,
                                                 Utility::PartitionBehavior::DISCARD_EMPTY),
                 std::runtime_error);
    EXPECT_THROW(Utility::PartitionElementCounts(numElements, maxBuckets,
                                                 Utility::PartitionBehavior::KEEP_EMPTY),
                 std::runtime_error);
}

TEST(Utility_PartitionElementCounts, shrink_to_fit_zero_num_elements)
{
    const int numElements = 0;
    const int maxBuckets = 5;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<int> expected;
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElementCounts, shrink_to_fit_smaller_num_elements)
{
    const int numElements = 3;
    const int maxBuckets = 5;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<int> expected{1, 1, 1};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElementCounts, shrink_to_fit_larger_num_elements)
{
    const int numElements = 20;
    const int maxBuckets = 3;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<int> expected{7, 7, 6};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElementCounts, shrink_to_fit_num_elements_multiple_of_max_buckets)
{
    const int numElements = 20;
    const int maxBuckets = 5;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<int> expected{4, 4, 4, 4, 4};
}

TEST(Utility_PartitionElementCounts, keep_empty_zero_num_elements)
{
    const int numElements = 0;
    const int maxBuckets = 5;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<int> expected{0, 0, 0, 0, 0};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElementCounts, keep_empty_smaller_num_elements)
{
    const int numElements = 3;
    const int maxBuckets = 5;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<int> expected{1, 1, 1, 0, 0};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElementCounts, keep_empty_larger_num_elements)
{
    const int numElements = 20;
    const int maxBuckets = 3;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<int> expected{7, 7, 6};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElementCounts, keep_empty_num_elements_multiple_of_max_buckets)
{
    const int numElements = 20;
    const int maxBuckets = 5;
    const auto result = Utility::PartitionElementCounts(numElements, maxBuckets,
                                                        Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<int> expected{4, 4, 4, 4, 4};
    EXPECT_EQ(expected, result);
}

// ------------------------
// PartitionElements
// ------------------------

TEST(Utility_PartitionElements, throws_on_negative_max_bucket_count)
{
    const std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const int maxBuckets = -1;
    EXPECT_THROW(
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::DISCARD_EMPTY),
        std::runtime_error);
    EXPECT_THROW(
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::KEEP_EMPTY),
        std::runtime_error);
}

TEST(Utility_PartitionElements, throws_on_zero_max_bucket_count)
{
    const std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const int maxBuckets = 0;
    EXPECT_THROW(
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::DISCARD_EMPTY),
        std::runtime_error);
    EXPECT_THROW(
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::KEEP_EMPTY),
        std::runtime_error);
}

TEST(Utility_PartitionElements, shrink_to_fit_zero_num_elements)
{
    const std::vector<int> input;
    const int maxBuckets = 5;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<std::vector<int>> expected;
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, shrink_to_fit_empty_smaller_input)
{
    const std::vector<int> input{1, 2, 3};
    const int maxBuckets = 5;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<std::vector<int>> expected{{1}, {2}, {3}};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, shrink_to_fit_larger_input)
{
    const std::vector<int> input{1, 2, 3, 4, 5, 6};
    const int maxBuckets = 4;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<std::vector<int>> expected{{1, 2}, {3, 4}, {5}, {6}};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, shrink_to_fit_num_elements_multiple_of_max_buckets)
{
    const std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8};
    const int maxBuckets = 4;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::DISCARD_EMPTY);
    const std::vector<std::vector<int>> expected{{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, keep_empty_zero_num_elements)
{
    const std::vector<int> input;
    const int maxBuckets = 5;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<std::vector<int>> expected(5, std::vector<int>{});
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, keep_empty_smaller_input)
{
    const std::vector<int> input{1, 2, 3};
    const int maxBuckets = 5;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<std::vector<int>> expected{{1}, {2}, {3}, {}, {}};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, keep_empty_larger_input)
{
    const std::vector<int> input{1, 2, 3, 4, 5, 6};
    const int maxBuckets = 4;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<std::vector<int>> expected{{1, 2}, {3, 4}, {5}, {6}};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, keep_empty_num_elements_multiple_of_max_buckets)
{
    const std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8};
    const int maxBuckets = 4;
    const auto result =
        Utility::PartitionElements(input, maxBuckets, Utility::PartitionBehavior::KEEP_EMPTY);
    const std::vector<std::vector<int>> expected{{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, input_string_vector)
{
    const std::vector<std::string> input{"one", "two", "three", "four", "five", "six"};
    const int maxBuckets = 4;
    const auto result = Utility::PartitionElements(input, maxBuckets);
    const std::vector<std::vector<std::string>> expected{
        {"one", "two"}, {"three", "four"}, {"five"}, {"six"}};
    EXPECT_EQ(expected, result);
}

TEST(Utility_PartitionElements, input_user_type)
{
    struct UserType
    {
        int i = 0;
        float f = 0.0f;
        bool operator==(const UserType& other) const noexcept
        {
            return std::tie(i, f) == std::tie(other.i, other.f);
        }
    };

    const std::vector<UserType> input{{1, 1.0f}, {2, 2.0f}, {3, 3.0f},
                                      {4, 4.0f}, {5, 5.0f}, {6, 6.0f}};
    const int maxBuckets = 4;
    const auto result = Utility::PartitionElements(input, maxBuckets);
    const std::vector<std::vector<UserType>> expected{
        {{{1, 1.0f}, {2, 2.0f}}}, {{{3, 3.0f}, {4, 4.0f}}}, {{5, 5.0f}}, {{6, 6.0f}}};

    ASSERT_EQ(result.size(), expected.size());
    for (std::size_t i = 0; i < result.size(); ++i) {
        const auto& o = result[i];
        const auto& e = expected[i];
        EXPECT_EQ(o, e);
    }
}

TEST(Utility_PartitionElements, rvalue_partition)
{
    std::vector<std::string> input{"one", "two", "three", "four", "five", "six"};
    const int maxBuckets = 4;
    const auto result = Utility::PartitionElements(std::move(input), maxBuckets);
    const std::vector<std::vector<std::string>> expected{
        {"one", "two"}, {"three", "four"}, {"five"}, {"six"}};
    EXPECT_EQ(expected, result);
}
