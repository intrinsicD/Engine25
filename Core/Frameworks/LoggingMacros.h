//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_LOGGINGMACROS_H
#define ENGINE25_LOGGINGMACROS_H

#include "Logger.h"

namespace Bcg{
    // Logging macros
#ifdef DEBUG
#define LOG_INFO(msg) Logger::GetInstance().Log(Logger::Level::Info, msg)
#else
#define LOG_INFO(msg) ((void)0) // No-op in release mode
#endif
#define LOG_WARN(msg) Logger::GetInstance().Log(Logger::Level::Warn, msg)
#define LOG_ERROR(msg) Logger::GetInstance().Log(Logger::Level::Error, msg)
#define LOG_FATAL(msg) Logger::GetInstance().Log(Logger::Level::Fatal, msg)
}

#endif //ENGINE25_LOGGINGMACROS_H
