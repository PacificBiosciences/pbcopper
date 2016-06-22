
#include <pbcopper/logging/Logging.h>
#include <boost/algorithm/string/predicate.hpp>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
using namespace PacBio;
using namespace PacBio::Logging;
using namespace std;

//
// NOTE: We need to scope the Loggers used here and invoke the macros that take
//       an explicit logger (rather than the convenience, default
//       singleton-logger macros). Essentially, the message queue is not
//       guaranteed to be processed until the logger goes out of scope. That's
//       usually at program exit, and no problem for production applications...
//       but we need finer control for testing here.
//

static const string infoMsg   = "*** Application INFO ***";
static const string noticeMsg = "*** Application NOTE ***";
static const string warnMsg   = "*** Application WARNING ***";


TEST(Logging_Logger, info_message_logged_to_stream)
{
    stringstream s;
    {
        Logger logger(s, LogLevel::INFO);
        PBLOGGER_INFO(logger) << infoMsg;
    }
    EXPECT_TRUE(s.str().find(infoMsg) != string::npos);
}

TEST(Logging_Logger, custom_logging_sinks_receive_expected_messages)
{
    // specify custom output destinations by log level... and also 'tee' one
    // log level stream (warning) into separate ostreams

    stringstream info;
    stringstream notice;
    stringstream warn, warn2;
    map<LogLevel, OStreams> logConfig =
    {
        {LogLevel::INFO,   {info}},
        {LogLevel::NOTICE, {notice}},
        {LogLevel::WARN,   {warn, warn2}}
    };

    {
        Logger logger(logConfig);
        PBLOGGER_INFO(logger)   << infoMsg;
        PBLOGGER_NOTICE(logger) << noticeMsg;
        PBLOGGER_WARN(logger)   << warnMsg;
    }
    EXPECT_TRUE(info.str().find(infoMsg)     != string::npos);
    EXPECT_TRUE(notice.str().find(noticeMsg) != string::npos);
    EXPECT_TRUE(warn.str().find(warnMsg)     != string::npos);
    EXPECT_TRUE(warn2.str().find(warnMsg)    != string::npos);
}
