#include "device_a.h"
#include "device_b.h"
#include "logger.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>

std::atomic<bool> running{ true };

void signalHandler(int) {
    running = false;
    std::cout << "\nReceived shutdown signal. Stopping...\n";
}

int main() {
    // Обработка Ctrl+C
    std::signal(SIGINT, signalHandler);

    // Инициализация логгера
    Logger::init("logs/system.log");
    Logger::log(Logger::Level::INFO, "Application starting", "MAIN");

    try {
        // Создание виртуального канала и устройств
        VirtualSerial serial;
        DeviceA deviceA(serial);
        DeviceB deviceB(serial);

        // Запуск устройств
        deviceA.start();
        deviceB.start();

        Logger::log(Logger::Level::INFO,
            "Both devices started. System is running.",
            "MAIN");

        // Главный цикл
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Остановка устройств
        Logger::log(Logger::Level::INFO, "Stopping devices...", "MAIN");
        deviceA.stop();
        deviceB.stop();

    }
    catch (const std::exception& e) {
        Logger::log(Logger::Level::ERROR,
            "Exception: " + std::string(e.what()),
            "MAIN");
        return 1;
    }

    Logger::log(Logger::Level::INFO, "Application stopped", "MAIN");
    Logger::shutdown();

    return 0;
}