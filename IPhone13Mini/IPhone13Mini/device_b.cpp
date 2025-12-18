#include "device_b.h"
#include <chrono>
#include <thread>

DeviceB::DeviceB(VirtualSerial& serial)
    : serial(serial), gen(rd()) {
    Logger::log(Logger::Level::INFO, "Device B initialized", "DEVICE_B");
}

DeviceB::~DeviceB() {
    stop();
}

void DeviceB::start() {
    if (!running) {
        running = true;
        thread = std::thread(&DeviceB::run, this);
        Logger::log(Logger::Level::INFO, "Device B started", "DEVICE_B");
    }
}

void DeviceB::stop() {
    if (running) {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
        Logger::log(Logger::Level::INFO, "Device B stopped", "DEVICE_B");
    }
}

void DeviceB::run() {
    while (running) {
        std::vector<uint8_t> data;
        if (serial.readB(data)) {
            Packet pkt;
            if (Packet::deserialize(data, pkt)) {
                processCommand(pkt);
            }
            else {
                Logger::log(Logger::Level::WARNING,
                    "Received invalid packet from A",
                    "DEVICE_B");
            }
        }

        // Небольшая пауза для уменьшения нагрузки на CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void DeviceB::processCommand(const Packet& pkt) {
    Logger::log(Logger::Level::DEBUG,
        "Processing command: 0x" + std::to_string((int)pkt.command_id),
        "DEVICE_B");

    switch (pkt.command_id) {
    case 0x10: { // Установить PWM
        if (pkt.payload.size() >= 2) {
            uint16_t pwmValue = Packet::bytesToUint16(pkt.payload.data());
            Logger::log(Logger::Level::INFO,
                "Setting PWM to: " + std::to_string(pwmValue),
                "DEVICE_B");
            sendPWMResponse(pwmValue);
        }
        break;
    }
    case 0x20: { // Установить PID
        if (pkt.payload.size() >= 12) { // 3 * float
            float kp = Packet::bytesToFloat(pkt.payload.data());
            float ki = Packet::bytesToFloat(pkt.payload.data() + 4);
            float kd = Packet::bytesToFloat(pkt.payload.data() + 8);

            Logger::log(Logger::Level::INFO,
                "Setting PID: kp=" + std::to_string(kp) +
                ", ki=" + std::to_string(ki) +
                ", kd=" + std::to_string(kd),
                "DEVICE_B");
            sendPIDResponse(kp, ki, kd);
        }
        break;
    }
    case 0x30: { // Запрос статуса
        Logger::log(Logger::Level::INFO,
            "Processing status request",
            "DEVICE_B");
        sendStatusResponse();
        break;
    }
    default:
        Logger::log(Logger::Level::WARNING,
            "Unknown command: 0x" + std::to_string((int)pkt.command_id),
            "DEVICE_B");
    }
}

void DeviceB::sendPWMResponse(uint16_t pwmValue) {
    Packet pkt;
    pkt.command_id = 0x81; // Подтверждение PWM
    pkt.payload = Packet::uint16ToBytes(pwmValue);

    auto data = pkt.serialize();
    serial.sendBtoA(data);
}

void DeviceB::sendPIDResponse(float kp, float ki, float kd) {
    Packet pkt;
    pkt.command_id = 0x82; // Подтверждение PID

    auto kpBytes = Packet::floatToBytes(kp);
    auto kiBytes = Packet::floatToBytes(ki);
    auto kdBytes = Packet::floatToBytes(kd);

    pkt.payload.insert(pkt.payload.end(), kpBytes.begin(), kpBytes.end());
    pkt.payload.insert(pkt.payload.end(), kiBytes.begin(), kiBytes.end());
    pkt.payload.insert(pkt.payload.end(), kdBytes.begin(), kdBytes.end());

    auto data = pkt.serialize();
    serial.sendBtoA(data);
}

void DeviceB::sendStatusResponse() {
    Packet pkt;
    pkt.command_id = 0x83; // Статус

    // Генерация случайных значений статуса
    std::uniform_int_distribution<> batteryDist(0, 100);
    std::uniform_int_distribution<> tempDist(20, 50);

    uint8_t battery = static_cast<uint8_t>(batteryDist(gen));
    uint8_t temperature = static_cast<uint8_t>(tempDist(gen));

    pkt.payload = { battery, temperature };

    auto data = pkt.serialize();
    serial.sendBtoA(data);
}