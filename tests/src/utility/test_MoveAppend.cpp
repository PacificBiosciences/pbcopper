#include <pbcopper/utility/MoveAppend.h>

#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace MoveAppendTests {

template <typename T, typename U>
auto CheckLValueAppend(T& src1, T& src2, U& dest, const U& expected)
{
    PacBio::Utility::MoveAppend(src1, dest);
    PacBio::Utility::MoveAppend(src2, dest);
    ASSERT_EQ(dest.size(), expected.size());
    EXPECT_TRUE(src1.empty());
    EXPECT_TRUE(src2.empty());
    EXPECT_EQ(dest, expected);
}

template <typename T, typename U>
auto CheckRValueAppend(T&& src1, T&& src2, U& dest, const U& expected)
{
    PacBio::Utility::MoveAppend(std::move(src1), dest);
    PacBio::Utility::MoveAppend(std::move(src2), dest);
    ASSERT_EQ(dest.size(), expected.size());
    EXPECT_TRUE(src1.empty());
    EXPECT_TRUE(src2.empty());
    EXPECT_EQ(dest, expected);
}

}  // namespace MoveAppendTests

TEST(Utility_MoveAppend, can_append_from_empty_source)
{
    {
        std::vector<int> src1;
        std::vector<int> src2;
        std::vector<int> dest{0, 1, 2};
        const std::vector<int> expected{0, 1, 2};
        MoveAppendTests::CheckLValueAppend(src1, src2, dest, expected);
    }
    {
        std::vector<int> src1;
        std::vector<int> src2;
        std::vector<int> dest{0, 1, 2};
        const std::vector<int> expected{0, 1, 2};
        MoveAppendTests::CheckRValueAppend(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveAppend, can_append_into_empty_destination)
{
    {
        std::vector<int> src1{0, 2, 4};
        std::vector<int> src2{1, 3, 5};
        std::vector<int> dest;
        const std::vector<int> expected{0, 2, 4, 1, 3, 5};
        MoveAppendTests::CheckLValueAppend(src1, src2, dest, expected);
    }
    {
        std::vector<int> src1{0, 2, 4};
        std::vector<int> src2{1, 3, 5};
        std::vector<int> dest;
        const std::vector<int> expected{0, 2, 4, 1, 3, 5};
        MoveAppendTests::CheckRValueAppend(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveAppend, can_append_vectors)
{
    {
        std::vector<int> src1{0, 2, 4};
        std::vector<int> src2{1, 3, 5};
        std::vector<int> dest{20, 30, 40};
        const std::vector<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveAppendTests::CheckLValueAppend(src1, src2, dest, expected);
    }
    {
        std::vector<int> src1{0, 2, 4};
        std::vector<int> src2{1, 3, 5};
        std::vector<int> dest{20, 30, 40};
        const std::vector<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveAppendTests::CheckRValueAppend(std::move(src1), std::move(src2), dest, expected);
    }
}

TEST(Utility_MoveAppend, can_append_strings)
{
    {
        std::string hello{"hello "};
        std::string world{"world!"};
        std::string dest;
        const std::string expected{"hello world!"};
        MoveAppendTests::CheckLValueAppend(hello, world, dest, expected);
    }
    {
        std::string hello{"hello "};
        std::string world{"world!"};
        std::string dest;
        const std::string expected{"hello world!"};
        MoveAppendTests::CheckRValueAppend(std::move(hello), std::move(world), dest, expected);
    }
}

TEST(Utility_MoveAppend, can_append_from_different_container_types)
{
    {
        std::set<int> src1{0, 2, 4};
        std::set<int> src2{1, 3, 5};
        std::vector<int> dest{20, 30, 40};
        const std::vector<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveAppendTests::CheckLValueAppend(src1, src2, dest, expected);
    }
    {
        std::set<int> src1{0, 2, 4};
        std::set<int> src2{1, 3, 5};
        std::vector<int> dest{20, 30, 40};
        const std::vector<int> expected{20, 30, 40, 0, 2, 4, 1, 3, 5};
        MoveAppendTests::CheckRValueAppend(std::move(src1), std::move(src2), dest, expected);
    }
}
