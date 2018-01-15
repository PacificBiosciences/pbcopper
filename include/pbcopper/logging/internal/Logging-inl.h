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

inline LogLevel::operator unsigned char(void) const { return value_; }

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

inline LogMessage::~LogMessage(void)
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
