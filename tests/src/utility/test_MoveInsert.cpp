#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/utility/MoveInsert.h>

namespace MoveInsertTests {

template <typename S1_t, typename S2_t, typename Dest_t>
auto CheckLValueInsert(S1_t& src1, S2_t& src2, Dest_t& dest, const Dest_t& expected)
{
    PacBio::Utility::MoveInsert(src1, dest);
    PacBio::Utility::MoveInsert(src2, dest);
    ASSERT_EQ(dest.size(), expected.size());
    EXPECT_TRUE(src1.empty());
    EXPECT_TRUE(src2.empty());
    EXPECT_EQ(dest, expected);
}

template <typename S1_t, typename S2_t, typename Dest_t>
auto CheckRValueInsert(S1_t&& src1, S2_t&& src2, Dest_t& dest, const Dest_t& expected)
{
    PacBio::Utility::MoveInsert(std::move(src1), dest);
    PacBio::Utility::MoveInsert(std::move(src2), dest);
    ASSERT_EQ(dest.size(), expected.size());
    EXPECT_TRUE(src1.empty());
    EXPECT_TRUE(src2.empty());
    EXPECT_EQ(dest, expected);
}

}  // namespace MoveInsertTests

TEST(Utility_MoveInsert, can_insert_from_empty_source)
{
    {
        std::set<int> src1;
        std::set<int> src2;
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::set<int> src1;
        std::set<int> src2;
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_into_empty_destination)
{
    {
        std::set<int> src1{0, 2, 4};
        std::set<int> src2{1, 3, 5, 0};
        std::set<int> dest;
        const std::set<int> expected{0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::set<int> src1{0, 2, 4};
        std::set<int> src2{1, 3, 5, 0};
        std::set<int> dest;
        const std::set<int> expected{0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_sets)
{
    {
        std::set<int> src1{0, 2, 4};
        std::set<int> src2{1, 3, 5, 0};
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::set<int> src1{0, 2, 4};
        std::set<int> src2{1, 3, 5, 0};
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_multisets)
{
    {
        std::multiset<int> src1{0, 2, 4};
        std::multiset<int> src2{1, 3, 5, 0};
        std::multiset<int> dest{20, 30, 40};
        const std::multiset<int> expected{20, 30, 40, 0, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::multiset<int> src1{0, 2, 4};
        std::multiset<int> src2{1, 3, 5, 0};
        std::multiset<int> dest{20, 30, 40};
        const std::multiset<int> expected{20, 30, 40, 0, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_unordered_sets)
{
    {
        std::unordered_set<int> src1{0, 2, 4};
        std::unordered_set<int> src2{1, 3, 5, 0};
        std::unordered_set<int> dest{20, 30, 40};
        const std::unordered_set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::unordered_set<int> src1{0, 2, 4};
        std::unordered_set<int> src2{1, 3, 5, 0};
        std::unordered_set<int> dest{20, 30, 40};
        const std::unordered_set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_unordered_multisets)
{
    {
        std::unordered_multiset<int> src1{0, 2, 4};
        std::unordered_multiset<int> src2{1, 3, 5, 0};
        std::unordered_multiset<int> dest{20, 30, 40};
        const std::unordered_multiset<int> expected{20, 30, 40, 0, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::unordered_multiset<int> src1{0, 2, 4};
        std::unordered_multiset<int> src2{1, 3, 5, 0};
        std::unordered_multiset<int> dest{20, 30, 40};
        const std::unordered_multiset<int> expected{20, 30, 40, 0, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

// clang-format off
TEST(Utility_MoveInsert, can_insert_maps)
{
    {
        std::map<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::map<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::map<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::map<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}
        };
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::map<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::map<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::map<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::map<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}
        };
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_multimaps)
{
    {
        std::multimap<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::multimap<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::multimap<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::multimap<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}, {"oscar", 2}
        };
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::multimap<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::multimap<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::multimap<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::multimap<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}, {"oscar", 2}
        };
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_unordered_maps)
{
    {
        std::unordered_map<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::unordered_map<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::unordered_map<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::unordered_map<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}
        };
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::unordered_map<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::unordered_map<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::unordered_map<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::unordered_map<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}
        };
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_unordered_multimaps)
{
    {
        std::unordered_multimap<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::unordered_multimap<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::unordered_multimap<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::unordered_multimap<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}, {"oscar", 2}
        };
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::unordered_multimap<std::string, int> src1{{"charlie", 1}, {"alpha", 3}, {"tango", 5}};
        std::unordered_multimap<std::string, int> src2{{"delta", 0}, {"oscar", 2}, {"golf", 6}};
        std::unordered_multimap<std::string, int> dest{{"november", 5}, {"oscar", 2}};
        const std::unordered_multimap<std::string, int> expected{
            {"charlie", 1}, {"alpha", 3}, {"tango", 5},
            {"delta", 0},   {"oscar", 2}, {"golf", 6},
            {"november", 5}, {"oscar", 2}
        };
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveInsert, can_insert_from_different_container_types)
{
    {
        std::vector<int> src1{0, 2, 4};
        std::vector<int> src2{1, 3, 5, 0};
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::vector<int> src1{0, 2, 4};
        std::vector<int> src2{1, 3, 5, 0};
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
    {
        std::vector<int> src1{0, 2, 4};
        std::unordered_set<int> src2{1, 3, 5, 0};
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckLValueInsert(src1, src2, dest, expected);
    }
    {
        std::vector<int> src1{0, 2, 4};
        std::unordered_set<int> src2{1, 3, 5, 0};
        std::set<int> dest{20, 30, 40};
        const std::set<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveInsertTests::CheckRValueInsert(std::move(src1), std::move(src2), dest, expected);
    }
}

// clang-format off
