#include "protocol.h"
#include <stdexcept>
#include <algorithm>

// Реализация CRC-16/X25 (перегрузка для вектора)
uint16_t Packet::calculateCRC(const std::vector<uint8_t>& data) {
    return calculateCRC(data.data(), data.size());
}

// Реализация CRC-16/X25 (для указателя и длины)
uint16_t Packet::calculateCRC(const uint8_t* data, size_t length) {
    // Параметры CRC-16/X25:
    // Полином: 0x1021, начальное значение: 0xFFFF
    // Отражение входа: true, отражение выхода: true
    // Выходной XOR: 0xFFFF

    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;

        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            }
            else {
                crc <<= 1;
            }
        }
    }

    // Инвертируем биты и применяем XOR
    crc = ~crc;
    return crc ^ 0xFFFF;
}

std::vector<uint8_t> Packet::serialize() const {
    std::vector<uint8_t> result;

    // Заголовок пакета (без CRC)
    result.push_back(HEADER);
    result.push_back(VERSION);
    result.push_back(command_id);
    result.push_back(static_cast<uint8_t>(payload.size()));

    // Полезная нагрузка
    result.insert(result.end(), payload.begin(), payload.end());

    // Расчет CRC для всех байт кроме последних 2
    uint16_t crc = calculateCRC(result);

    // Добавляем CRC (little-endian)
    result.push_back(static_cast<uint8_t>(crc & 0xFF));
    result.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

    return result;
}

bool Packet::deserialize(const std::vector<uint8_t>& data, Packet& out) {
    // Минимальный размер: header(1) + version(1) + cmd(1) + len(1) + crc(2) = 6
    if (data.size() < 6) {
        return false;
    }

    // Проверка заголовка и версии
    if (data[0] != HEADER || data[1] != VERSION) {
        return false;
    }

    uint8_t payload_len = data[3];

    // Проверка длины пакета
    if (data.size() != 6 + payload_len) {
        return false;
    }

    // Проверка CRC
    std::vector<uint8_t> dataWithoutCRC(data.begin(), data.end() - 2);
    uint16_t received_crc = static_cast<uint16_t>(data[data.size() - 2]) |
        (static_cast<uint16_t>(data[data.size() - 1]) << 8);

    uint16_t calculated_crc = calculateCRC(dataWithoutCRC);

    if (received_crc != calculated_crc) {
        return false;
    }

    // Заполнение структуры
    out.command_id = data[2];
    out.payload.assign(data.begin() + 4, data.begin() + 4 + payload_len);

    return true;
}

std::vector<uint8_t> Packet::floatToBytes(float value) {
    std::vector<uint8_t> bytes(sizeof(float));
    std::memcpy(bytes.data(), &value, sizeof(float));
    return bytes;
}

float Packet::bytesToFloat(const uint8_t* bytes) {
    float value;
    std::memcpy(&value, bytes, sizeof(float));
    return value;
}

std::vector<uint8_t> Packet::uint16ToBytes(uint16_t value) {
    return {
        static_cast<uint8_t>(value & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF)
    };
}

uint16_t Packet::bytesToUint16(const uint8_t* bytes) {
    return static_cast<uint16_t>(bytes[0]) |
        (static_cast<uint16_t>(bytes[1]) << 8);
}