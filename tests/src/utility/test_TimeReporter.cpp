#include <pbcopper/utility/TimeReporter.h>

#include <functional>
#include <sstream>

#include <gtest/gtest.h>

#include <pbcopper/logging/Logging.h>

using namespace PacBio;

namespace TimeReporterTests {

using TestFunction = std::function<void(void)>;

std::string TestLog(const PacBio::Logging::LogLevel loggerLevel, const TestFunction& func)
{
    std::ostringstream out;
    {
        // setup local logger
        Logging::Logger logger{out, loggerLevel};
        Logging::Logger::Current(&logger);

        // run test block
        func();

        // reset default logging
        Logging::Logger::Current(&Logging::Logger::Default());
    }

    // return log output
    return out.str();
}

}  // namespace TimeReporterTests

// clang-format off

TEST(Utility_TimeReporter, can_log_default_message)
{
    const Logging::LogLevel runtimeLogLevel = Logging::LogLevel::INFO;
    const std::string output = TimeReporterTests::TestLog(runtimeLogLevel, []()
    {
        Utility::TimeReporter tr{Logging::LogLevel::INFO};
    });

    EXPECT_TRUE(output.find("Finished in ") != std::string::npos);
}

TEST(Utility_TimeReporter, does_not_log_on_incompatible_log_level)
{
    const Logging::LogLevel runtimeLogLevel = Logging::LogLevel::WARN;
    const std::string output = TimeReporterTests::TestLog(runtimeLogLevel, []()
    {
        Utility::TimeReporter tr{Logging::LogLevel::INFO};
    });

    EXPECT_TRUE(output.empty());
}

TEST(Utility_TimeReporter, can_log_using_custom_callback_printer)
{
    const Logging::LogLevel runtimeLogLevel = Logging::LogLevel::DEBUG;
    const std::string output = TimeReporterTests::TestLog(runtimeLogLevel, []()
    {
        int count = 0;
        double score = 0.0;

        auto ScoreReport = [&](const double nsec) {
            std::ostringstream msg;
            msg << "Found " << count << " alignments with average score "
                << score << " in " << Utility::Stopwatch::PrettyPrintNanoseconds(nsec);
            return msg.str();
        };

        Utility::TimeReporter tr{Logging::LogLevel::INFO, ScoreReport};

        // task updates some values before the reporter prints in its dtor
        count = 42;
        score = 0.77;
    });

    EXPECT_TRUE(output.find("Found 42 alignments with average score 0.77 in ") !=
                std::string::npos);
}

// clang-format on
