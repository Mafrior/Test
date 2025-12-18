#pragma once

#include <string>
#include <mutex>
#include <fstream>
#include <chrono>
#include <iomanip>

class Logger {
public:
    enum class Level { INFO, WARNING, ERROR, DEBUG };

    // Инициализация логгера (открытие файла)
    static void init(const std::string& filename = "logs/system.log");

    // Основной метод логирования
    static void log(Level level, const std::string& message, const std::string& source = "");

    // Закрытие логгера
    static void shutdown();

private:
    static std::mutex logMutex;
    static std::ofstream logFile;
    static bool initialized;

    static std::string levelToString(Level level);
    static std::string currentTime();
};