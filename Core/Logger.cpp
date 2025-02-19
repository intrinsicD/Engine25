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

    std::string GetTimestamp() {
        auto now = std::time(nullptr);
        std::ostringstream timestamp;
        timestamp << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
        return timestamp.str();
    }

    std::string ToString(Logger::Level level) {
        switch (level) {
            case Logger::Level::TODO:
                return fmt::format("{}", fmt::styled(" TODO", fmt::fg(fmt::color::yellow)));
            case Logger::Level::Info:
                return fmt::format("{}", fmt::styled(" INFO", fmt::fg(fmt::color::green)));
            case Logger::Level::Warn:
                return fmt::format("{}", fmt::styled(" WARN", fmt::fg(fmt::color::orange)));
            case Logger::Level::Error:
                return fmt::format("{}", fmt::styled("ERROR", fmt::fg(fmt::color::red)));
            case Logger::Level::Fatal:
                return fmt::format("{}", fmt::styled("FATAL", fmt::fg(fmt::color::blue)));
            default:
                return fmt::format("{}", fmt::styled("UNKNOWN", fmt::fg(fmt::color::pink)));
        }
    }

    Logger &Logger::get_instance() {
        static Logger instance;
        return instance;
    }

    void Logger::set_log_file(const std::string &filePath) {
        std::lock_guard<std::mutex> lock(mutex_);
        logFile_.open(filePath, std::ios::out | std::ios::app);
        //add a header to the log file
        logFile_ << "Log file created at " << GetTimestamp() << "\n";
    }

    void Logger::log(Level level, const std::string &message) {
        std::lock_guard<std::mutex> lock(mutex_);

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

    void Logger::enable_console_logger(bool enable) {
        logToConsole_ = enable;
    }

    Logger::Logger() : logToConsole_(true) {}

    Logger::~Logger() {
        if (logFile_.is_open()) {
            //add a footer to the log file
            logFile_ << "Log file closed at " << GetTimestamp() << "\n\n";
            logFile_.close();
        }
    }

}