// Author: Lance Hepler, Derek Barnett

#ifndef PBCOPPER_LOGGING_LOGGING_INL_H
#define PBCOPPER_LOGGING_LOGGING_INL_H

#include <cstddef>
#include <sstream>
#include <stdexcept>

#include <pbcopper/logging/Logging.h>

namespace PacBio {
namespace Logging {

// ----------
// LogLevel
// ----------

inline LogLevel::LogLevel(const unsigned char value) : value_{value} {}

inline LogLevel::operator unsigned char() const { return value_; }

// --------------
// LoggerConfig
// --------------

inline LoggerConfig::LoggerConfig(const std::map<LogLevel, OStreams>& cfg)
    : std::map<LogLevel, OStreams>(cfg)
{
}

inline LoggerConfig::LoggerConfig(const std::map<std::string, OStreams>& cfg)
    : std::map<LogLevel, OStreams>()
{
    for (const auto& kv : cfg)
        (*this)[kv.first] = kv.second;
}

inline LoggerConfig::LoggerConfig(std::ostream& os, const LogLevel level)
{
    for (size_t i = static_cast<size_t>(level); i < LogLevel::MAX_LOG_LEVEL; ++i)
        (*this)[static_cast<LogLevel>(i)].push_back(os);
}

inline LoggerConfig::LoggerConfig(std::ostream& os, const std::string& level)
    : LoggerConfig(os, LogLevel(level))
{
}

// ---------
// Logger
// ---------

template <typename... Args>
Logger::Logger(Args&&... args)
    : cfg_(std::forward<Args>(args)...), writer_(&Logger::MessageWriter, this)
{
#ifdef NDEBUG
    if (Handles(LogLevel::TRACE))
        throw std::invalid_argument("one cannot simply log TRACE messages in release builds!");
#endif
}

inline bool Logger::Handles(const LogLevel level) const
{
    return cfg_.find(level) != cfg_.end() && !cfg_.at(level).empty();
}

// ------------
// LogMessage
// ------------

inline LogMessage::~LogMessage()
{
    if (ptr_) logger_ << std::move(ptr_);
}

template <typename T>
inline LogMessage& LogMessage::operator<<(const T& t)
{
    if (ptr_) std::get<1>(*ptr_) << t;
    return *this;
}

}  // namespace Logging
}  // namespace PacBio

#endif  // PBCOPPER_LOGGING_LOGGING_INL_H
