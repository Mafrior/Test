#pragma once

#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>

class VirtualSerial {
public:
    // Отправка данных от A к B
    void sendAtoB(const std::vector<uint8_t>& data);

    // Отправка данных от B к A
    void sendBtoA(const std::vector<uint8_t>& data);

    // Чтение данных для A (неблокирующее)
    bool readA(std::vector<uint8_t>& out);

    // Чтение данных для B (неблокирующее)
    bool readB(std::vector<uint8_t>& out);

private:
    std::deque<std::vector<uint8_t>> queueAtoB;
    std::deque<std::vector<uint8_t>> queueBtoA;

    mutable std::mutex mutex;
    std::condition_variable cvA;
    std::condition_variable cvB;
};