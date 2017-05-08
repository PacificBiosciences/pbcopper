#include "pbcopper/logging/Logging.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <csignal>
#include <ctime>
using namespace PacBio;
using namespace PacBio::Logging;

namespace PacBio {
namespace Logging {
namespace internal {

const char* LogLevelRepr(LogLevel level)
{
    // by specification these are all of length 10
    switch (level) {
        case LogLevel::TRACE:    return "TRACE     ";
        case LogLevel::DEBUG:    return "DEBUG     ";
        case LogLevel::INFO:     return "INFO      ";
        case LogLevel::NOTICE:   return "NOTICE    ";
        case LogLevel::WARN:     return "WARN      ";
        case LogLevel::ERROR:    return "ERROR     ";
        case LogLevel::CRITICAL: return "CRITICAL  ";
        case LogLevel::FATAL:    return "FATAL     ";
        default:                 return "OTHER     ";
    }
}

LogLevel LogLevelFromString(const std::string& level)
{
    if (level == "TRACE")    return LogLevel::TRACE;
    if (level == "DEBUG")    return LogLevel::DEBUG;
    if (level == "INFO")     return LogLevel::INFO;
    if (level == "NOTICE")   return LogLevel::NOTICE;
    if (level == "ERROR")    return LogLevel::ERROR;
    if (level == "CRITICAL") return LogLevel::CRITICAL;
    if (level == "FATAL")    return LogLevel::FATAL;
    if (level == "WARN" || level == "WARNING")
        return LogLevel::WARN;

    throw std::invalid_argument("invalid log level");
}

} // namespace internal

void InstallSignalHandlers(Logger& logger)
{
    using std::raise;
    using std::signal;

    static Logger& logger_ = logger;

    // catch in-flight exceptions on terminate
    std::set_terminate([]() {
        if (auto eptr = std::current_exception()) {
            try {
                std::rethrow_exception(eptr);
            } catch (const std::exception& e) {
                PBLOGGER_FATAL(logger_) << "caught exception: \"" << e.what() << '"';
            } catch (...) {
                PBLOGGER_FATAL(logger_) << "caught unknown exception type";
            }
        }
        // call the SIGABRT handler (below)
        std::abort();
    });

    // register signal handlers that log and then re-raise as normal

    signal(SIGABRT, [](int) {
        {
            PBLOGGER_FATAL(logger_) << "caught SIGABRT";
        }
        logger_.~Logger();
        signal(SIGABRT, SIG_DFL);
        raise(SIGABRT);
    });
    signal(SIGINT, [](int) {
        {
            PBLOGGER_FATAL(logger_) << "caught SIGINT";
        }
        logger_.~Logger();
        signal(SIGINT, SIG_DFL);
        raise(SIGINT);
    });
    signal(SIGSEGV, [](int) {
        {
            PBLOGGER_FATAL(logger_) << "caught SIGSEGV";
        }
        logger_.~Logger();
        signal(SIGSEGV, SIG_DFL);
        raise(SIGSEGV);
    });
    signal(SIGTERM, [](int) {
        {
            PBLOGGER_FATAL(logger_) << "caught SIGTERM";
        }
        logger_.~Logger();
        signal(SIGTERM, SIG_DFL);
        raise(SIGTERM);
    });
}

} // namespace Logging
} // namespace PacBio

LogLevel::LogLevel(const std::string& value)
    : value_{ internal::LogLevelFromString(value) }
{ }

Logger::~Logger(void)
{
    if (!writer_.joinable())
        throw std::runtime_error("this logger is already dead!");

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
    if (!writer_.joinable())
        throw std::runtime_error("this logger is dead!");
    {
        std::lock_guard<std::mutex> g(m_);
        queue_.emplace(std::forward<std::unique_ptr<LogLevelStream>>(ptr));
    }
    pushed_.notify_all();
    return *this;
}

Logger& Logger::Default(Logger* logger)
{
    static std::unique_ptr<Logger> logger_(new Logger(std::cerr));
    if (logger) logger_.reset(logger);
    return *logger_;
}

void Logger::MessageWriter(void)
{
    while (true) {
        std::unique_ptr<LogLevelStream> ptr;

        // wait on messages to arrive in the queue_, and pop them off
        {
            std::unique_lock<std::mutex> lk(m_);
            pushed_.wait(lk, [&ptr, this]() {
                if (queue_.empty())
                    return false;
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
        if (cfg_.find(level) != cfg_.end()) {
            for (const auto& os : cfg_.at(level))
                os.get() << std::get<1>(*ptr).str() << std::endl;
        }

        // and notify flush we delivered a message to os_,
        popped_.notify_all();
    }
}

LogMessage::LogMessage(const char* file,
                              const char* function,
                              unsigned int line,
                              const LogLevel level,
                              Logger& logger)
    : logger_(logger)
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    if (!logger_.Handles(level))
        return;

    ptr_.reset(new Logger::LogLevelStream(std::piecewise_construct,
                                          std::forward_as_tuple(level),
                                          std::forward_as_tuple()));

    static const char* delim = " -|- ";

    // get the time, separated into seconds and milliseconds
    const auto now  = system_clock::now();
    const auto secs = duration_cast<seconds>(now.time_since_epoch());
    const auto time = system_clock::to_time_t(system_clock::time_point(secs));
    const auto msec = duration_cast<milliseconds>(now.time_since_epoch() - secs).count();

    // format the time and print out the log header to the ostringstream
    // TODO(lhepler) make this std::put_time when we move to gcc-5
    char buf[20];
    struct tm gmTime;
    std::strftime(buf, 20, "%Y%m%d %T.", gmtime_r(&time, &gmTime));

    std::get<1>(*ptr_) << ">|> " << buf << std::setfill('0') << std::setw(3)
                       << std::to_string(msec) << delim << internal::LogLevelRepr(level) << delim
                       << function
#ifndef NDEBUG
                       << " at " << file << ':' << line
#endif
                       << delim << std::hex << std::showbase << std::this_thread::get_id()
                       << std::noshowbase << std::dec << "||" << delim;

#ifdef NDEBUG
    UNUSED(file);
    UNUSED(line);
#endif
}
