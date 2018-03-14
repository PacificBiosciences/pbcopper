// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Lance Hepler, Derek Barnett

#ifndef PBCOPPER_LOGGING_LOGGING_H
#define PBCOPPER_LOGGING_LOGGING_H

#include <condition_variable>
#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Logging {

class LogLevel
{
public:
    enum : unsigned char
    {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        NOTICE = 3,
        WARN = 4,
        ERROR = 5,
        CRITICAL = 6,
        FATAL = 7,

        MAX_LOG_LEVEL = 8
    };

public:
    LogLevel(const unsigned char value);
    LogLevel(const std::string& value);

public:
    operator unsigned char(void) const;

private:
    unsigned char value_;
};

typedef std::reference_wrapper<std::ostream> OStreamWrapper;
typedef std::vector<OStreamWrapper> OStreams;

class LoggerConfig : public std::map<LogLevel, OStreams>
{
public:
    LoggerConfig(const std::map<LogLevel, OStreams>& cfg);
    LoggerConfig(const std::map<std::string, OStreams>& cfg);
    LoggerConfig(std::ostream& os, const LogLevel level = LogLevel::INFO);
    LoggerConfig(std::ostream& os, const std::string& level);
};

class LogMessage;

class Logger
{
private:
    typedef std::pair<LogLevel, std::ostringstream> LogLevelStream;

public:
    static Logger& Default(Logger* logger = nullptr);

public:
    template <typename... Args>
    Logger(Args&&... args);

    Logger(const Logger& other) = delete;
    ~Logger(void);

private:
    LoggerConfig cfg_;
    std::mutex m_;
    std::condition_variable popped_;
    std::condition_variable pushed_;
    std::queue<std::unique_ptr<LogLevelStream>> queue_;
    std::thread writer_;
    friend class LogMessage;

private:
    Logger& operator<<(std::unique_ptr<LogLevelStream>&& ptr);
    bool Handles(const LogLevel level) const;
    void MessageWriter(void);
};

class LogMessage
{
public:
    LogMessage(const char* file, const char* function, unsigned int line, const LogLevel level,
               Logger& logger);
    LogMessage(const LogMessage& msg) = delete;
    ~LogMessage(void);

public:
    template <typename T>
    LogMessage& operator<<(const T& t);

private:
    std::unique_ptr<Logger::LogLevelStream> ptr_;
    Logger& logger_;
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

extern void InstallSignalHandlers(Logger& logger = Logger::Default());

}  // namespace Logging
}  // namespace PacBio

#include <pbcopper/logging/internal/Logging-inl.h>

#endif  // PBCOPPER_LOGGING_LOGGING_H
