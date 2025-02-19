//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_LOGGER_H
#define ENGINE25_LOGGER_H

#include <mutex>
#include <fstream>
#include "fmt/core.h"

namespace Bcg {
    class Logger {
    public:
        enum class Level {
            TODO,
            Info,
            Warn,
            Error,
            Fatal,
        };
        
        static Logger &get_instance();

        void set_log_file(const std::string &filePath);

        void log(Level level, const std::string &message);

        void enable_console_logger(bool enable);

    private:
        Logger();

        ~Logger();

        std::ofstream logFile_;
        std::mutex mutex_;
        bool logToConsole_;
    };


    using FrameLogFunction = void (*)(Bcg::Logger::Level, const std::string &);
    using InfoLogFunction = void (*)(Bcg::Logger::Level, const std::string &);
    using WarnLogFunction = void (*)(Bcg::Logger::Level, const std::string &);
    using ErrorLogFunction = void (*)(Bcg::Logger::Level, const std::string &);
    using FatalLogFunction = void (*)(Bcg::Logger::Level, const std::string &);

    inline void RealLog(Logger::Level level, const std::string &msg) {
        Bcg::Logger::get_instance().log(level, msg);
    }

    // No-op logging function
    inline void NoOpLog(Logger::Level, const std::string &) {
        // Do nothing
    }

    inline InfoLogFunction g_currentInfoLogFunc = RealLog;
    inline WarnLogFunction g_currentWarnLogFunc = RealLog;
    inline ErrorLogFunction g_currentErrorLogFunc = RealLog;
    inline FatalLogFunction g_currentFatalLogFunc = RealLog;
    inline FrameLogFunction g_currentFrameLogFunc = NoOpLog;

#define LOG_TODO(msg) (RealLog(Logger::Level::TODO, msg))
#define LOG_INFO(msg) (g_currentInfoLogFunc(Logger::Level::Info, (msg)))
#define LOG_WARN(msg) (g_currentWarnLogFunc(Logger::Level::Warn, (msg)))
#define LOG_ERROR(msg) (g_currentErrorLogFunc(Logger::Level::Error, (msg)))
#define LOG_FATAL(msg) (g_currentFatalLogFunc(Logger::Level::Fatal, (msg)))
#define LOG_FRAME(msg) (g_currentFrameLogFunc(Logger::Level::Info, (msg)))

    inline void EnableInfoLogging(bool enable) {
        g_currentInfoLogFunc = enable ? RealLog : NoOpLog;
    }

    inline void EnableWarnLogging(bool enable) {
        g_currentWarnLogFunc = enable ? RealLog : NoOpLog;
    }

    inline void EnableErrorLogging(bool enable) {
        g_currentErrorLogFunc = enable ? RealLog : NoOpLog;
    }

    inline void EnableFatalLogging(bool enable) {
        g_currentFatalLogFunc = enable ? RealLog : NoOpLog;
    }

    inline void EnableFrameLogging(bool enable) {
        g_currentFrameLogFunc = enable ? RealLog : NoOpLog;
    }
}

#endif //ENGINE25_LOGGER_H
