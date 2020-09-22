#include <pbcopper/utility/Stopwatch.h>

#include <chrono>

#include <gtest/gtest.h>

TEST(Utility_Stopwatch, can_pretty_print_time)
{
    auto prettyPrintHours = [](int hours) {
        return PacBio::Utility::Stopwatch::PrettyPrintNanoseconds(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::hours(hours))
                .count());
    };

    EXPECT_EQ("4h ", prettyPrintHours(4));
    EXPECT_EQ("4d ", prettyPrintHours(4 * 24));
    EXPECT_EQ("4d 1h ", prettyPrintHours(4 * 24 + 1));
}
