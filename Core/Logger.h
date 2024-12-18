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
        
        static Logger &GetInstance();

        void SetLogFile(const std::string &filePath);

        void SetLogLevel(Level level);

        void Log(Level level, const std::string &message);

        void EnableConsoleLogger(bool enable);

    private:
        Logger();

        ~Logger();

        std::string GetTimestamp() const;

        std::string ToString(Level level) const;

        std::ofstream logFile_;
        std::mutex mutex_;
        Level minLogLevel_;
        bool logToConsole_;
    };

    // Logging macros
#ifdef DEBUG
#define LOG_INFO(msg) Logger::GetInstance().Log(Logger::Level::Info, msg)
#else
#define LOG_INFO(msg) ((void)0) // No-op in release mode
#endif
#define LOG_TODO(msg) Logger::GetInstance().Log(Logger::Level::TODO, msg)
#define LOG_WARN(msg) Logger::GetInstance().Log(Logger::Level::Warn, msg)
#define LOG_ERROR(msg) Logger::GetInstance().Log(Logger::Level::Error, msg)
#define LOG_FATAL(msg) Logger::GetInstance().Log(Logger::Level::Fatal, msg)

    using FrameLogFunction = void (*)(Bcg::Logger::Level, const std::string &);

    inline void RealLog(Logger::Level level, const std::string &msg) {
        Bcg::Logger::GetInstance().Log(level, msg);
    }

    // No-op logging function
    inline void NoOpLog(Logger::Level, const std::string &) {
        // Do nothing
    }

    inline FrameLogFunction g_currentLogFunc = NoOpLog;

#define LOG_FRAME(msg) (g_currentLogFunc(Logger::Level::Info, (msg)))

    inline void EnableFrameLogging(bool enable) {
        g_currentLogFunc = enable ? RealLog : NoOpLog;
    }
}

#endif //ENGINE25_LOGGER_H
