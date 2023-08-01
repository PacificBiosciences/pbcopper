#include <pbcopper/logging/Logging.h>

#include <pbcopper/utility/Unused.h>

#include <chrono>
#include <iostream>
#include <ostream>

#include <cassert>
#include <cstddef>
#include <ctime>

namespace PacBio {
namespace Logging {

// ---------
// Logger
// ---------

Logger::Logger(std::ostream& out, const LogLevel level) : Logger{out, LogConfig{level}} {}

Logger::Logger(const std::filesystem::path& filename, const LogLevel level)
    : Logger{filename, LogConfig{level}}
{}

Logger::Logger(std::ostream& out, const LogConfig& config)
    : stream_{out}, config_{config}, writer_{&Logger::MessageWriter, this}
{}

Logger::Logger(const std::filesystem::path& filename, const LogConfig& config)
    : logFile_{filename}, stream_{logFile_}, config_{config}, writer_{&Logger::MessageWriter, this}
{}

Logger::~Logger()
{
    if (!writer_.joinable()) {
        return;
    }

    // place a terminal sentinel for MessageWriter to know it's done
    {
        std::lock_guard<std::mutex> g(m_);
        queue_.emplace(std::unique_ptr<LogLevelStream>());
    }
    pushed_.notify_all();

    // wait for everything to be flushed
    {
        std::unique_lock<std::mutex> lk(m_);
        popped_.wait(lk, [this]() { return queue_.empty(); });
        // endl implicitly flushes, so no need to call os_.flush() here
    }

    // join writer thread
    writer_.join();
}

Logger& Logger::operator<<(std::unique_ptr<LogLevelStream>&& ptr)
{
    if (!writer_.joinable()) {
        throw std::runtime_error("this logger is dead!");
    }
    {
        std::lock_guard<std::mutex> g(m_);
        queue_.emplace(std::forward<std::unique_ptr<LogLevelStream>>(ptr));
    }
    pushed_.notify_all();
    return *this;
}

Logger& Logger::Current(Logger* newLogger)
{
    // NOTE: non-owning
    static Logger* currentLogger = nullptr;

    // If a new logger is explicitly provided, use it.
    if (newLogger) {
        currentLogger = newLogger;
    } else {
        // If we don't have a current logger set yet, create via Default().
        // Default() maintains its own logger's lifetime.
        if (!currentLogger) {
            currentLogger = &(Default());
        }
    }
    return *currentLogger;
}

LogLevel CurrentLogLevel() { return Logger::Current().Level(); }

std::int32_t CurrentLeftBlockWidth() { return Logger::Current().LeftBlockWidth(); }

bool CurrentAlignLevel() { return Logger::Current().AlignLevel(); }

Logger& Logger::Default(Logger* logger)
{
    static auto logger_ = std::make_unique<Logger>(std::cerr, LogLevel::INFO);
    if (logger) {
        logger_.reset(logger);
    }
    return *logger_;
}

bool Logger::Handles(const LogLevel level) const { return level >= config_.Level; }

LogLevel Logger::Level() const { return config_.Level; }

std::int32_t Logger::LeftBlockWidth() const { return config_.LeftBlockWidth; }

bool Logger::AlignLevel() const { return config_.AlignLevel; }

void Logger::MessageWriter()
{
    while (true) {
        std::unique_ptr<LogLevelStream> ptr;

        // wait on messages to arrive in the queue_, and pop them off
        {
            std::unique_lock<std::mutex> lk(m_);
            pushed_.wait(lk, [&ptr, this]() {
                if (queue_.empty()) {
                    return false;
                }
                ptr = std::move(queue_.front());
                queue_.pop();
                return true;
            });
        }

        // if we've reached the null terminator, notify flush and stop
        if (!ptr) {
            popped_.notify_all();
            break;
        }

        // otherwise, push the message onto os_
        const LogLevel level = std::get<0>(*ptr);
        if (Handles(level)) {
            auto& os = stream_.get();
            os << std::get<1>(*ptr).str() << std::endl;
        }

        // and notify flush we delivered a message to os_,
        popped_.notify_all();
    }
}

void LogBlock(const char* file, const char* function, unsigned int line,
              const std::string_view left, const std::string_view right,
              const Logging::LogLevel logLevel)
{
    if (Logging::CurrentLogLevel() > logLevel) {
        return;
    }

    std::int32_t LogWidthLeftSideAdjusted = Logging::CurrentLeftBlockWidth();
    if (!Logging::CurrentAlignLevel()) {
        switch (logLevel) {
            case Logging::LogLevel::INFO:
            case Logging::LogLevel::WARN:
                LogWidthLeftSideAdjusted -= 4;
                break;
            case Logging::LogLevel::ERROR:
            case Logging::LogLevel::FATAL:
            case Logging::LogLevel::TRACE:
            case Logging::LogLevel::DEBUG:
                LogWidthLeftSideAdjusted -= 5;
                break;
            case Logging::LogLevel::NOTICE:
                LogWidthLeftSideAdjusted -= 6;
                break;
            case Logging::LogLevel::VERBOSE:
                LogWidthLeftSideAdjusted -= 7;
                break;
            case Logging::LogLevel::CRITICAL:
                LogWidthLeftSideAdjusted -= 8;
                break;
            default:
                break;
        }
    }

    if (right.empty()) {
        PacBio::Logging::LogMessage(file, function, line, logLevel,
                                    PacBio::Logging::Logger::Current())
            << left;
    } else {
        PacBio::Logging::LogMessage(file, function, line, logLevel,
                                    PacBio::Logging::Logger::Current())
            << std::setw(LogWidthLeftSideAdjusted) << std::setfill(' ') << std::left << left
            << " : " << right;
    }
}

}  // namespace Logging
}  // namespace PacBio
