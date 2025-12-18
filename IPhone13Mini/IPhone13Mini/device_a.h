#pragma once

#include "virtual_serial.h"
#include "protocol.h"
#include "logger.h"
#include <atomic>
#include <thread>
#include <random>

class DeviceA {
public:
    DeviceA(VirtualSerial& serial);
    ~DeviceA();

    void start();
    void stop();

private:
    void run();
    void sendPWMCommand();
    void sendStatusRequest();
    void processResponse(const std::vector<uint8_t>& data);

    VirtualSerial& serial;
    std::thread thread;
    std::atomic<bool> running{ false };
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> cmdDist{ 0, 1 }; // 0 - PWM, 1 - Status
    std::uniform_int_distribution<uint16_t> pwmDist{ 0, 65535 };
};