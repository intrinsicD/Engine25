//
// Created by alex on 04.12.24.
//

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Logger.h"
#include "fmt/color.h"

namespace Bcg {
    Logger &Logger::GetInstance() {
        static Logger instance;
        return instance;
    }

    void Logger::SetLogFile(const std::string &filePath) {
        std::lock_guard<std::mutex> lock(mutex_);
        logFile_.open(filePath, std::ios::out | std::ios::app);
        //add a header to the log file
        logFile_ << "Log file created at " << GetTimestamp() << "\n";
    }

    void Logger::SetLogLevel(Level level) {
        minLogLevel_ = level;
    }

    void Logger::Log(Level level, const std::string &message) {
        std::lock_guard<std::mutex> lock(mutex_);

        // Filter out messages below the minimum log level
        if (level < minLogLevel_) return;

        // Format the log message
        std::ostringstream logStream;
        logStream << "[" << GetTimestamp() << "] [" << ToString(level) << "] " << message << "\n";

        // Output to console
        if (logToConsole_) {
            std::cout << logStream.str();
        }

        // Output to file
        if (logFile_.is_open()) {
            logFile_ << logStream.str();
        }

        // Fatal log handling
        if (level == Level::Fatal) {
            std::cerr << "Fatal error encountered. Terminating.\n";
            std::terminate();
        }
    }

    void Logger::EnableConsoleLogger(bool enable) {
        logToConsole_ = enable;
    }

    Logger::Logger() : minLogLevel_(Level::Info), logToConsole_(true) {}

    Logger::~Logger() {
        if (logFile_.is_open()) {
            //add a footer to the log file
            logFile_ << "Log file closed at " << GetTimestamp() << "\n\n";
            logFile_.close();
        }
    }

    std::string Logger::GetTimestamp() const {
        auto now = std::time(nullptr);
        std::ostringstream timestamp;
        timestamp << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
        return timestamp.str();
    }

    std::string Logger::ToString(Level level) const {
        switch (level) {
            case Level::TODO:
                return fmt::format("{}", fmt::styled(" TODO", fmt::fg(fmt::color::yellow)));
            case Level::Info:
                return fmt::format("{}", fmt::styled(" INFO", fmt::fg(fmt::color::green)));
            case Level::Warn:
                return fmt::format("{}", fmt::styled(" WARN", fmt::fg(fmt::color::orange)));
            case Level::Error:
                return fmt::format("{}", fmt::styled("ERROR", fmt::fg(fmt::color::red)));
            case Level::Fatal:
                return fmt::format("{}", fmt::styled("FATAL", fmt::fg(fmt::color::blue)));
            default:
                return fmt::format("{}", fmt::styled("UNKNOWN", fmt::fg(fmt::color::pink)));
        }
    }
}