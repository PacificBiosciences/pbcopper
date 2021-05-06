#include <pbcopper/logging/Logging.h>

#include <sstream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>

#include <gtest/gtest.h>

#include <pbcopper/utility/StringUtils.h>

using namespace PacBio;

//
// NOTE: We need to scope the Loggers used here and invoke the macros that take
//       an explicit logger (rather than the convenience, default
//       singleton-logger macros). Essentially, the message queue is not
//       guaranteed to be processed until the logger goes out of scope. That's
//       usually at program exit, and no problem for production applications...
//       but we need finer control for testing here.
//

namespace LoggingTests {

static const std::string debugMsg{"*** Application DEBUG ***"};
static const std::string verboseMsg{"*** Application VERBOSE ***"};
static const std::string infoMsg{"*** Application INFO ***"};
static const std::string warnMsg{"*** Application WARNING ***"};

}  // namespace LoggingTests

TEST(Logging_LogLevel, can_be_constructed_from_string)
{
    const Logging::LogLevel info{"INFO"};
    EXPECT_EQ(Logging::LogLevel::INFO, info);

    const Logging::LogLevel warn{"WARN"};
    EXPECT_EQ(Logging::LogLevel::WARN, warn);

    EXPECT_THROW(Logging::LogLevel(""), std::invalid_argument);
    EXPECT_THROW(Logging::LogLevel("bad"), std::invalid_argument);
}

TEST(Logging_LogLevel, can_be_converted_to_string)
{
    const Logging::LogLevel info{Logging::LogLevel::INFO};
    const Logging::LogLevel warn{Logging::LogLevel::WARN};

    EXPECT_EQ("INFO", info.ToString());
    EXPECT_EQ("WARN", warn.ToString());
}

TEST(Logging_Logger, writes_normal_info_message_to_output_stream)
{
    std::ostringstream s;
    {
        Logging::Logger logger{s, Logging::LogLevel::INFO};
        PBLOGGER_INFO(logger) << LoggingTests::infoMsg;
        PBLOGGER_VERBOSE(logger) << LoggingTests::verboseMsg;
    }

    const std::string result = s.str();
    EXPECT_NE(std::string::npos, result.find(LoggingTests::infoMsg));
    EXPECT_EQ(std::string::npos, result.find(LoggingTests::verboseMsg));  // not written
}

TEST(Logging_Logger, writes_normal_verbose_message_to_output_stream)
{
    std::ostringstream s;
    {
        Logging::Logger logger{s, Logging::LogLevel::VERBOSE};
        PBLOGGER_VERBOSE(logger) << LoggingTests::verboseMsg;
    }

    const std::string result = s.str();
    EXPECT_NE(std::string::npos, result.find(LoggingTests::verboseMsg));
}

TEST(Logging_Logger, ignores_lower_level_messages)
{
    std::ostringstream s;
    {
        Logging::Logger logger{s, Logging::LogLevel::INFO};
        PBLOGGER_DEBUG(logger) << LoggingTests::debugMsg;
        PBLOGGER_VERBOSE(logger) << LoggingTests::verboseMsg;
    }
    EXPECT_TRUE(s.str().empty());
}

TEST(Logging_Logger, can_write_message_only_custom_field_config)
{
    {
        std::ostringstream s;
        {
            Logging::LogConfig config;
            config.Fields = Logging::LogField::NONE;

            Logging::Logger logger(s, config);
            PBLOGGER_WARN(logger) << LoggingTests::warnMsg;
        }
        EXPECT_EQ(">|> *** Application WARNING ***\n", s.str());
    }
    {
        std::ostringstream s;
        {
            Logging::LogConfig config;
            config.Fields = Logging::LogField::LOG_LEVEL;

            Logging::Logger logger(s, config);
            PBLOGGER_WARN(logger) << LoggingTests::warnMsg;
        }
        EXPECT_EQ(">|> WARN -|- *** Application WARNING ***\n", s.str());
    }
    {
        std::ostringstream s;
        {
            Logging::LogConfig config;
            config.Fields = Logging::LogField::LOG_LEVEL | Logging::LogField::THREAD_ID;

            Logging::Logger logger(s, config);
            PBLOGGER_ERROR(logger) << "Message";
        }
        EXPECT_NE(std::string::npos, s.str().find("ERROR"));
        EXPECT_NE(std::string::npos, s.str().find("Message"));
        EXPECT_NE(std::string::npos, s.str().find("0x"));
    }
}

TEST(Logging_Logger, can_use_custom_delimiter)
{
    std::ostringstream s;
    {
        Logging::LogConfig config;
        config.Fields = Logging::LogField::LOG_LEVEL;
        config.Delimiter = " :: ";

        Logging::Logger logger(s, config);
        PBLOGGER_WARN(logger) << LoggingTests::warnMsg;
    }
    EXPECT_EQ(">|> WARN :: *** Application WARNING ***\n", s.str());
}

TEST(Logging_Logger, trace_level_is_a_noop_in_release_mode)
{
    std::ostringstream s;
    int x = 0;
    {
        const Logging::LogConfig config{Logging::LogLevel::TRACE};
        Logging::Logger logger(s, config);

        auto incremented = [](int& y) {
            ++y;
            return std::to_string(y);  // just something to feed the logger
        };

        PBLOGGER_TRACE(logger) << incremented(x) << incremented(x);
        PBLOGGER_TRACE(logger) << incremented(x) << incremented(x);
    }

#ifdef NDEBUG
    EXPECT_EQ(0, x);
#else
    EXPECT_EQ(4, x);
#endif  // NDEBUG
}

TEST(Logging_Logging, can_query_active_log_level)
{
    {
        Logging::Logger logger{std::cerr, Logging::LogLevel::INFO};
        Logging::Logger::Current(&logger);

        EXPECT_EQ(Logging::LogLevel::INFO, Logging::CurrentLogLevel());

        // reset default logging
        Logging::Logger::Current(&Logging::Logger::Default());
    }
    {
        Logging::Logger logger{std::cerr, Logging::LogLevel::DEBUG};
        Logging::Logger::Current(&logger);

        EXPECT_EQ(Logging::LogLevel::DEBUG, Logging::CurrentLogLevel());

        // reset default logging
        Logging::Logger::Current(&Logging::Logger::Default());
    }
}
