#pragma once

#include <vector>
#include <cstdint>
#include <cstring>

struct Packet {
    static constexpr uint8_t HEADER = 0xAA;
    static constexpr uint8_t VERSION = 0x01;

    uint8_t command_id;
    std::vector<uint8_t> payload;

    std::vector<uint8_t> serialize() const;
    static bool deserialize(const std::vector<uint8_t>& data, Packet& out);
    static uint16_t calculateCRC(const std::vector<uint8_t>& data);
    static uint16_t calculateCRC(const uint8_t* data, size_t length);  // Перегруженный метод

    static std::vector<uint8_t> floatToBytes(float value);
    static float bytesToFloat(const uint8_t* bytes);
    static std::vector<uint8_t> uint16ToBytes(uint16_t value);
    static uint16_t bytesToUint16(const uint8_t* bytes);
};