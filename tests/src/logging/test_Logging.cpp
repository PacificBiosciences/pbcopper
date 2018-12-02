#include <pbcopper/logging/Logging.h>

#include <sstream>
#include <string>

#include <gtest/gtest.h>
#include <boost/algorithm/string/predicate.hpp>

//
// NOTE: We need to scope the Loggers used here and invoke the macros that take
//       an explicit logger (rather than the convenience, default
//       singleton-logger macros). Essentially, the message queue is not
//       guaranteed to be processed until the logger goes out of scope. That's
//       usually at program exit, and no problem for production applications...
//       but we need finer control for testing here.
//

namespace LoggingTests {

static const std::string infoMsg = "*** Application INFO ***";
static const std::string noticeMsg = "*** Application NOTE ***";
static const std::string warnMsg = "*** Application WARNING ***";

}  // namespace LoggingTests

TEST(Logging_Logger, info_message_logged_to_stream)
{
    std::ostringstream s;
    {
        PacBio::Logging::Logger logger(s, PacBio::Logging::LogLevel::INFO);
        PBLOGGER_INFO(logger) << LoggingTests::infoMsg;
    }
    EXPECT_TRUE(s.str().find(LoggingTests::infoMsg) != std::string::npos);
}

TEST(Logging_Logger, custom_logging_sinks_receive_expected_messages)
{
    // specify custom output destinations by log level... and also 'tee' one
    // log level stream (warning) into separate ostreams

    std::ostringstream info;
    std::ostringstream notice;
    std::ostringstream warn, warn2;
    std::map<PacBio::Logging::LogLevel, PacBio::Logging::OStreams> logConfig = {
        {PacBio::Logging::LogLevel::INFO, {info}},
        {PacBio::Logging::LogLevel::NOTICE, {notice}},
        {PacBio::Logging::LogLevel::WARN, {warn, warn2}}};

    {
        PacBio::Logging::Logger logger(logConfig);
        PBLOGGER_INFO(logger) << LoggingTests::infoMsg;
        PBLOGGER_NOTICE(logger) << LoggingTests::noticeMsg;
        PBLOGGER_WARN(logger) << LoggingTests::warnMsg;
    }
    EXPECT_TRUE(info.str().find(LoggingTests::infoMsg) != std::string::npos);
    EXPECT_TRUE(notice.str().find(LoggingTests::noticeMsg) != std::string::npos);
    EXPECT_TRUE(warn.str().find(LoggingTests::warnMsg) != std::string::npos);
    EXPECT_TRUE(warn2.str().find(LoggingTests::warnMsg) != std::string::npos);
}
