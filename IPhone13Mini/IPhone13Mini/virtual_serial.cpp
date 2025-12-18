#include "virtual_serial.h"

void VirtualSerial::sendAtoB(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex);
    queueAtoB.push_back(data);
    cvB.notify_one(); // Будим устройство B
}

void VirtualSerial::sendBtoA(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex);
    queueBtoA.push_back(data);
    cvA.notify_one(); // Будим устройство A
}

bool VirtualSerial::readA(std::vector<uint8_t>& out) {
    std::lock_guard<std::mutex> lock(mutex);
    if (queueBtoA.empty()) {
        return false;
    }
    out = queueBtoA.front();
    queueBtoA.pop_front();
    return true;
}

bool VirtualSerial::readB(std::vector<uint8_t>& out) {
    std::lock_guard<std::mutex> lock(mutex);
    if (queueAtoB.empty()) {
        return false;
    }
    out = queueAtoB.front();
    queueAtoB.pop_front();
    return true;
}