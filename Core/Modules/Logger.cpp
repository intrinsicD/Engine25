//
// Created by alex on 04.12.24.
//

#include "Logger.h"

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
            case Level::Info:
                return " INFO";
            case Level::Warn:
                return " WARN";
            case Level::Error:
                return "ERROR";
            case Level::Fatal:
                return "FATAL";
            default:
                return "UNKNOWN";
        }
    }
}