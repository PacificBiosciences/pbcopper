

#include <pbcopper/utility/Stopwatch.h>
#include <gtest/gtest.h>
#include <thread>

using namespace PacBio;
using namespace PacBio::Utility;

//
// using float compares and 'fuzzy' compares to allow for inherent wiggle room
// in system execution
//

TEST(Utility_Stopwatch, determines_elapsed_time_in_milliseconds)
{
    Stopwatch s;
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    auto elapsed = s.ElapsedMilliseconds();
    ASSERT_FLOAT_EQ(3, elapsed);
}

TEST(Utility_Stopwatch, determines_elapsed_time_in_seconds)
{
    // seconds
    Stopwatch s;
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    auto elapsed = s.ElapsedSeconds();
    ASSERT_FLOAT_EQ(3/1000, elapsed);
}

TEST(Utility_Stopwatch, determines_elapsed_time_in_user_precision)
{
    Stopwatch s;
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    auto elapsed = s.Elapsed<std::chrono::microseconds>();
    ASSERT_NEAR(3000, elapsed, 500);
}

TEST(Utility_Stopwatch, determines_elapsed_time_since_reset)
{
    Stopwatch s;
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    s.Reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    auto elapsed = s.ElapsedMilliseconds();
    ASSERT_FLOAT_EQ(3, elapsed);
}
