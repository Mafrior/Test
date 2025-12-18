#pragma once

#include "virtual_serial.h"
#include "protocol.h"
#include "logger.h"
#include <atomic>
#include <thread>
#include <random>

class DeviceB {
public:
    DeviceB(VirtualSerial& serial);
    ~DeviceB();

    void start();
    void stop();

private:
    void run();
    void processCommand(const Packet& pkt);
    void sendPWMResponse(uint16_t pwmValue);
    void sendPIDResponse(float kp, float ki, float kd);
    void sendStatusResponse();

    VirtualSerial& serial;
    std::thread thread;
    std::atomic<bool> running{ false };
    std::random_device rd;
    std::mt19937 gen;
};