//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_LOGGER_H
#define ENGINE25_LOGGER_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <ctime>
#include <iomanip>
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
}

#endif //ENGINE25_LOGGER_H
