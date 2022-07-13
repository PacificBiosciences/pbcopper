#include <pbcopper/container/Contains.h>

#include <gtest/gtest.h>

#include <set>
#include <vector>

using namespace PacBio;

TEST(Container_Contains, can_use_find_member_function)
{
    const std::set<int> data{1, 2, 3, 4, 5};
    EXPECT_TRUE(Container::Contains(data, 3));
    EXPECT_FALSE(Container::Contains(data, 10));
}

TEST(Container_Contains, uses_find_algorithm_fallback_if_no_find_member_function)
{
    const std::vector<int> data{1, 2, 3, 4, 5};
    EXPECT_TRUE(Container::Contains(data, 3));
    EXPECT_FALSE(Container::Contains(data, 10));
}

TEST(Container_Contains, can_check_custom_container)
{
    struct ContainsTestContainer
    {
        std::vector<int> Data;

        auto cbegin() const noexcept { return Data.cbegin(); }
        auto begin() const noexcept { return Data.begin(); }
        auto begin() noexcept { return Data.begin(); }
        auto cend() const noexcept { return Data.cend(); }
        auto end() const noexcept { return Data.end(); }
        auto end() noexcept { return Data.end(); }
    };

    ContainsTestContainer c;
    c.Data = {1, 2, 3, 4, 5};
    EXPECT_TRUE(Container::Contains(c, 3));
    EXPECT_FALSE(Container::Contains(c, 10));
}
