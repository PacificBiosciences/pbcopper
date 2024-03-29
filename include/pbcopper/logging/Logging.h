#ifndef PBCOPPER_LOGGING_LOGGING_H
#define PBCOPPER_LOGGING_LOGGING_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/logging/LogConfig.h>
#include <pbcopper/logging/LogLevel.h>
#include <pbcopper/logging/LogMessage.h>

#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <thread>

#include <cstdint>

namespace PacBio {
namespace Logging {

class Logger
{
public:
    ///
    /// Contains a non-owning pointer to the current logger in use. Client code
    /// is responsible for lifetime of newLogger, if provided.
    ///
    static Logger& Current(Logger* newLogger = nullptr);

    ///
    /// For legacy reasons, this maintains ownership of (and possibly creates)
    /// the default logger.
    ///
    static Logger& Default(Logger* logger = nullptr);

    Logger(std::ostream& out, LogLevel level);
    Logger(const std::filesystem::path& filename, LogLevel level);

    Logger(std::ostream& out, const LogConfig& config);
    Logger(const std::filesystem::path& filename, const LogConfig& config);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();

    LogLevel Level() const;
    std::int32_t LeftBlockWidth() const;
    bool AlignLevel() const;

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
    bool Handles(LogLevel level) const;
    void MessageWriter();
};

LogLevel CurrentLogLevel();

#define PBLOGGER_LEVEL(lg, lvl) \
    PacBio::Logging::LogMessage(__FILE__, __func__, __LINE__, PacBio::Logging::lvl, (lg))

// clang-format off
//
// Log message with desired log level & provided logger. TRACE level messages
// are disabled in Release mode.
//
#define PBLOGGER_DEBUG(lg)    PBLOGGER_LEVEL(lg, LogLevel::DEBUG)
#define PBLOGGER_VERBOSE(lg)  PBLOGGER_LEVEL(lg, LogLevel::VERBOSE)
#define PBLOGGER_INFO(lg)     PBLOGGER_LEVEL(lg, LogLevel::INFO)
#define PBLOGGER_NOTICE(lg)   PBLOGGER_LEVEL(lg, LogLevel::NOTICE)
#define PBLOGGER_WARN(lg)     PBLOGGER_LEVEL(lg, LogLevel::WARN)
#define PBLOGGER_ERROR(lg)    PBLOGGER_LEVEL(lg, LogLevel::ERROR)
#define PBLOGGER_CRITICAL(lg) PBLOGGER_LEVEL(lg, LogLevel::CRITICAL)
#define PBLOGGER_FATAL(lg)    PBLOGGER_LEVEL(lg, LogLevel::FATAL)

#ifdef NDEBUG
#define PBLOGGER_TRACE(lg) \
    if (false) PBLOGGER_LEVEL(lg, LogLevel::TRACE)
#else
#define PBLOGGER_TRACE(lg) PBLOGGER_LEVEL(lg, LogLevel::TRACE)
#endif

//
// Log message with desired log level & current logger. TRACE level messages
// are disabled in Release mode.
//
#define PBLOG_LEVEL(lvl) PBLOGGER_LEVEL(PacBio::Logging::Logger::Current(), lvl)

#define PBLOG_DEBUG    PBLOG_LEVEL(LogLevel::DEBUG)
#define PBLOG_VERBOSE  PBLOG_LEVEL(LogLevel::VERBOSE)
#define PBLOG_INFO     PBLOG_LEVEL(LogLevel::INFO)
#define PBLOG_NOTICE   PBLOG_LEVEL(LogLevel::NOTICE)
#define PBLOG_WARN     PBLOG_LEVEL(LogLevel::WARN)
#define PBLOG_ERROR    PBLOG_LEVEL(LogLevel::ERROR)
#define PBLOG_CRITICAL PBLOG_LEVEL(LogLevel::CRITICAL)
#define PBLOG_FATAL    PBLOG_LEVEL(LogLevel::FATAL)

#ifdef NDEBUG
#define PBLOG_TRACE \
    if (false) PBLOG_LEVEL(LogLevel::TRACE)
#else
#define PBLOG_TRACE PBLOG_LEVEL(LogLevel::TRACE)
#endif

// clang-format on

void LogBlock(const char* file, const char* function, unsigned int line, std::string_view left,
              std::string_view right = {}, Logging::LogLevel logLevel = Logging::LogLevel::INFO);

#define PBLOG_BLOCK_TRACE(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::TRACE) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::TRACE); \
    }

#define PBLOG_BLOCK_DEBUG(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::DEBUG) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::DEBUG); \
    }

#define PBLOG_BLOCK_VERBOSE(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::VERBOSE) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::VERBOSE); \
    }

#define PBLOG_BLOCK_INFO(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::INFO) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::INFO); \
    }

#define PBLOG_BLOCK_NOTICE(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::NOTICE) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::NOTICE); \
    }

#define PBLOG_BLOCK_WARN(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::WARN) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::WARN); \
    }

#define PBLOG_BLOCK_ERROR(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::ERROR) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::ERROR); \
    }

#define PBLOG_BLOCK_CRITICAL(left, right)                                                 \
    if (Logging::CurrentLogLevel() <= Logging::LogLevel::CRITICAL) {                      \
        LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::CRITICAL); \
    }

#define PBLOG_BLOCK_FATAL(left, right) \
    LogBlock(__FILE__, __func__, __LINE__, left, right, Logging::LogLevel::FATAL);

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGGING_H
