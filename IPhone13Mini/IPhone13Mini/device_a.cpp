#include "device_a.h"
#include <chrono>
#include <thread>

DeviceA::DeviceA(VirtualSerial& serial)
    : serial(serial), gen(rd()) {
    Logger::log(Logger::Level::INFO, "Device A initialized", "DEVICE_A");
}

DeviceA::~DeviceA() {
    stop();
}

void DeviceA::start() {
    if (!running) {
        running = true;
        thread = std::thread(&DeviceA::run, this);
        Logger::log(Logger::Level::INFO, "Device A started", "DEVICE_A");
    }
}

void DeviceA::stop() {
    if (running) {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
        Logger::log(Logger::Level::INFO, "Device A stopped", "DEVICE_A");
    }
}

void DeviceA::run() {
    while (running) {
        // Случайный выбор команды
        if (cmdDist(gen) == 0) {
            sendPWMCommand();
        }
        else {
            sendStatusRequest();
        }

        // Проверяем ответы от B
        std::vector<uint8_t> response;
        while (serial.readA(response)) {
            processResponse(response);
        }

        // Ждем 100 мс
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void DeviceA::sendPWMCommand() {
    uint16_t pwmValue = pwmDist(gen);
    Logger::log(Logger::Level::DEBUG,
        "Sending PWM command: " + std::to_string(pwmValue),
        "DEVICE_A");

    Packet pkt;
    pkt.command_id = 0x10; // Команда PWM
    pkt.payload = Packet::uint16ToBytes(pwmValue);

    auto data = pkt.serialize();
    serial.sendAtoB(data);
}

void DeviceA::sendStatusRequest() {
    Logger::log(Logger::Level::DEBUG,
        "Sending status request",
        "DEVICE_A");

    Packet pkt;
    pkt.command_id = 0x30; // Запрос статуса
    // Пустой payload

    auto data = pkt.serialize();
    serial.sendAtoB(data);
}

void DeviceA::processResponse(const std::vector<uint8_t>& data) {
    Packet pkt;
    if (!Packet::deserialize(data, pkt)) {
        Logger::log(Logger::Level::WARNING,
            "Received invalid packet from B",
            "DEVICE_A");
        return;
    }

    switch (pkt.command_id) {
    case 0x81: { // Подтверждение PWM
        if (pkt.payload.size() >= 2) {
            uint16_t pwmValue = Packet::bytesToUint16(pkt.payload.data());
            Logger::log(Logger::Level::INFO,
                "Received PWM acknowledgment: " + std::to_string(pwmValue),
                "DEVICE_A");
        }
        break;
    }
    case 0x82: { // Подтверждение PID
        Logger::log(Logger::Level::INFO,
            "Received PID acknowledgment",
            "DEVICE_A");
        break;
    }
    case 0x83: { // Статус
        if (pkt.payload.size() >= 2) {
            uint8_t battery = pkt.payload[0];
            uint8_t temperature = pkt.payload[1];
            Logger::log(Logger::Level::INFO,
                "Received status: battery=" + std::to_string((int)battery) +
                "%, temperature=" + std::to_string((int)temperature) + "°C",
                "DEVICE_A");
        }
        break;
    }
    default:
        Logger::log(Logger::Level::WARNING,
            "Unknown response command: 0x" +
            std::to_string((int)pkt.command_id),
            "DEVICE_A");
    }
}