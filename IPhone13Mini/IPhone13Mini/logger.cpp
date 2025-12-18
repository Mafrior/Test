#include "logger.h"
#include <iostream>
#include <sstream>
#include <chrono>

std::mutex Logger::logMutex;
std::ofstream Logger::logFile;
bool Logger::initialized = false;

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (!initialized) {
        // Создаем директорию для логов, если ее нет
        size_t pos = filename.find_last_of("/\\");
        if (pos != std::string::npos) {
            std::string dir = filename.substr(0, pos);
        }

        logFile.open(filename, std::ios::app);
        if (logFile.is_open()) {
            initialized = true;
            log(Level::INFO, "Logger initialized", "LOGGER");
        }
        else {
            // Если не удалось открыть файл, используем консоль
            std::cerr << "Failed to open log file: " << filename << std::endl;
            initialized = true; // Все равно помечаем как инициализированный
        }
    }
}

void Logger::log(Level level, const std::string& message, const std::string& source) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (!initialized) {
        // Если логгер не инициализирован, инициализируем с дефолтным файлом
        init("logs/system.log");
    }

    std::string logEntry = "[" + currentTime() + "] [" + levelToString(level) + "]";
    if (!source.empty()) {
        logEntry += " [" + source + "]";
    }
    logEntry += " " + message + "\n";

    // Запись в файл
    if (logFile.is_open()) {
        logFile << logEntry;
        logFile.flush();
    }

    std::cout << logEntry;
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        log(Level::INFO, "Logger shutting down", "LOGGER");
        logFile.close();
    }
    initialized = false;
}

std::string Logger::levelToString(Level level) {
    switch (level) {
    case Level::INFO: return "INFO";
    case Level::WARNING: return "WARN";
    case Level::ERROR: return "ERROR";
    case Level::DEBUG: return "DEBUG";
    default: return "UNKNOWN";
    }
}

std::string Logger::currentTime() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm tm;

    localtime_s(&tm, &time);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

    std::stringstream ss;
    ss << buffer << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}