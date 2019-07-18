// Author: Lance Hepler, Derek Barnett

#ifndef PBCOPPER_LOGGING_LOGGING_H
#define PBCOPPER_LOGGING_LOGGING_H

#include <pbcopper/PbcopperConfig.h>

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <pbcopper/logging/LogConfig.h>
#include <pbcopper/logging/LogLevel.h>
#include <pbcopper/logging/LogMessage.h>

namespace PacBio {
namespace Logging {

class Logger
{
public:
    static Logger& Default(Logger* logger = nullptr);

    Logger(std::ostream& out, const LogLevel level);
    Logger(const std::string& filename, const LogLevel level);

    Logger(std::ostream& out, const LogConfig& config);
    Logger(const std::string& filename, const LogConfig& config);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();

private:
    // output control
    std::ofstream logFile_;
    std::reference_wrapper<std::ostream> stream_;
    LogConfig config_;

    // thread control
    std::mutex m_;
    std::condition_variable popped_;
    std::condition_variable pushed_;
    std::queue<std::unique_ptr<LogLevelStream>> queue_;
    std::thread writer_;

    friend class LogMessage;

    Logger& operator<<(std::unique_ptr<LogLevelStream>&& ptr);
    bool Handles(const LogLevel level) const;
    void MessageWriter();
};

// trace is disabled under Release builds (-DNDEBUG)
#ifdef NDEBUG
#define PBLOGGER_LEVEL(lg, lvl)                                   \
    if (PacBio::Logging::lvl != PacBio::Logging::LogLevel::TRACE) \
    PacBio::Logging::LogMessage(__FILE__, __func__, __LINE__, PacBio::Logging::lvl, (lg))
#else
#define PBLOGGER_LEVEL(lg, lvl) \
    PacBio::Logging::LogMessage(__FILE__, __func__, __LINE__, PacBio::Logging::lvl, (lg))
#endif

//
// Log message with desired log level & provided logger
//
#define PBLOGGER_TRACE(lg) PBLOGGER_LEVEL(lg, LogLevel::TRACE)
#define PBLOGGER_DEBUG(lg) PBLOGGER_LEVEL(lg, LogLevel::DEBUG)
#define PBLOGGER_INFO(lg) PBLOGGER_LEVEL(lg, LogLevel::INFO)
#define PBLOGGER_NOTICE(lg) PBLOGGER_LEVEL(lg, LogLevel::NOTICE)
#define PBLOGGER_WARN(lg) PBLOGGER_LEVEL(lg, LogLevel::WARN)
#define PBLOGGER_ERROR(lg) PBLOGGER_LEVEL(lg, LogLevel::ERROR)
#define PBLOGGER_CRITICAL(lg) PBLOGGER_LEVEL(lg, LogLevel::CRITICAL)
#define PBLOGGER_FATAL(lg) PBLOGGER_LEVEL(lg, LogLevel::FATAL)

//
// Log message with desired log level & default logger
//
#define PBLOG_LEVEL(lvl) PBLOGGER_LEVEL(PacBio::Logging::Logger::Default(), lvl)

#define PBLOG_TRACE PBLOG_LEVEL(LogLevel::TRACE)
#define PBLOG_DEBUG PBLOG_LEVEL(LogLevel::DEBUG)
#define PBLOG_INFO PBLOG_LEVEL(LogLevel::INFO)
#define PBLOG_NOTICE PBLOG_LEVEL(LogLevel::NOTICE)
#define PBLOG_WARN PBLOG_LEVEL(LogLevel::WARN)
#define PBLOG_ERROR PBLOG_LEVEL(LogLevel::ERROR)
#define PBLOG_CRITICAL PBLOG_LEVEL(LogLevel::CRITICAL)
#define PBLOG_FATAL PBLOG_LEVEL(LogLevel::FATAL)

void InstallSignalHandlers(Logger& logger = Logger::Default());

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGGING_H
