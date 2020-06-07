#include <pbcopper/utility/Ssize.h>

#include <vector>

#include <gtest/gtest.h>

using namespace PacBio;

TEST(Utility_Ssize, can_get_ssize_of_vector)
{
    const std::vector<int> v{1, 2, 3};
    EXPECT_EQ(3, Utility::Ssize(v));
}

TEST(Utility_Ssize, can_get_ssize_of_array)
{
    const int a[]{1, 2, 3};
    EXPECT_EQ(3, Utility::Ssize(a));
}
