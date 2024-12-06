//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_LOGGINGMACROS_H
#define ENGINE25_LOGGINGMACROS_H

#include "Logger.h"

namespace Bcg {
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

#endif //ENGINE25_LOGGINGMACROS_H
