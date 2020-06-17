// Author: Lance Hepler, Derek Barnett

#include <pbcopper/logging/LogMessage.h>

#include <ctime>

#include <chrono>
#include <iomanip>
#include <ios>

#include <pbcopper/logging/Logging.h>
#include <pbcopper/utility/Unused.h>

namespace PacBio {
namespace Logging {

LogMessage::LogMessage(const char* file, const char* function, unsigned int line,
                       const LogLevel level, Logger& logger)
    : logger_{logger}
{
    if (!logger_.Handles(level)) return;
    ptr_.reset(new LogLevelStream(std::piecewise_construct, std::forward_as_tuple(level),
                                  std::forward_as_tuple()));

    const auto& config = logger.config_;
    auto useField = [&](int logField) { return (config.Fields & logField) != 0; };

    auto& out = ptr_->second;
    out << config.Header;

    if (useField(LogField::TIMESTAMP)) {

        using namespace std::chrono;

        // get the time, separated into seconds and milliseconds
        const auto now = system_clock::now();
        const auto secs = duration_cast<seconds>(now.time_since_epoch());
        const auto time = system_clock::to_time_t(system_clock::time_point(secs));
        const auto msec = duration_cast<milliseconds>(now.time_since_epoch() - secs).count();

        // format and output
        struct std::tm gmTime;
        out << std::put_time(gmtime_r(&time, &gmTime), "%Y%m%d %T.") << std::setfill('0')
            << std::setw(3) << std::to_string(msec) << config.Delimiter;
    }

    if (useField(LogField::LOG_LEVEL)) out << level.ToString() << config.Delimiter;

    if (useField(LogField::FUNCTION)) {
        out << function
#ifndef NDEBUG
            << " at " << file << ':' << line
#endif
            << config.Delimiter;
    }

    if (useField(LogField::THREAD_ID)) {
        out << std::hex << std::showbase << std::this_thread::get_id() << std::noshowbase
            << std::dec << "||" << config.Delimiter;
    }

#ifdef NDEBUG
    UNUSED(file);
    UNUSED(line);
#endif
}

LogMessage::~LogMessage()
{
    if (ptr_) logger_ << std::move(ptr_);
}

}  // namespace Logging
}  // namespace PacBio
