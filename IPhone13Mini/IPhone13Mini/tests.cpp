//#include "protocol.h"
//#include <iostream>
//#include <cassert>
//#include <cmath>
//
//void testCRC() {
//    std::cout << "Testing CRC calculation...\n";
//
//    // Тестовые данные
//    std::vector<uint8_t> testData = { 0xAA, 0x01, 0x10, 0x02, 0x00, 0x00 };
//    uint16_t crc = Packet::calculateCRC(testData);
//
//    // Проверяем, что CRC не равен 0
//    assert(crc != 0);
//    std::cout << "CRC for test data: 0x" << std::hex << crc << std::dec << "\n";
//
//    // Проверяем, что CRC для одинаковых данных одинаков
//    uint16_t crc2 = Packet::calculateCRC(testData);
//    assert(crc == crc2);
//
//    // Тест с пустым вектором
//    std::vector<uint8_t> empty;
//    uint16_t crcEmpty = Packet::calculateCRC(empty);
//    std::cout << "CRC for empty data: 0x" << std::hex << crcEmpty << std::dec << "\n";
//
//    std::cout << "CRC test passed!\n\n";
//}
//
//void testPacketSerialization() {
//    std::cout << "Testing packet serialization...\n";
//
//    // Создаем тестовый пакет
//    Packet original;
//    original.command_id = 0x10;
//    original.payload = { 0x34, 0x12 }; // PWM value 0x1234
//
//    // Сериализуем
//    auto serialized = original.serialize();
//
//    // Проверяем базовую структуру
//    assert(serialized.size() >= 6);
//    assert(serialized[0] == Packet::HEADER);
//    assert(serialized[1] == Packet::VERSION);
//    assert(serialized[2] == original.command_id);
//    assert(serialized[3] == original.payload.size());
//
//    // Десериализуем
//    Packet deserialized;
//    bool success = Packet::deserialize(serialized, deserialized);
//
//    assert(success);
//    assert(deserialized.command_id == original.command_id);
//    assert(deserialized.payload == original.payload);
//
//    std::cout << "Packet serialization test passed!\n\n";
//}
//
//void testFloatConversion() {
//    std::cout << "Testing float conversion...\n";
//
//    float original = 3.14159f;
//    auto bytes = Packet::floatToBytes(original);
//    float converted = Packet::bytesToFloat(bytes.data());
//
//    assert(std::abs(original - converted) < 0.0001f);
//
//    std::cout << "Float conversion test passed! "
//        << original << " -> " << converted << "\n\n";
//}
//
//void testUint16Conversion() {
//    std::cout << "Testing uint16 conversion...\n";
//
//    uint16_t original = 0x1234;
//    auto bytes = Packet::uint16ToBytes(original);
//    uint16_t converted = Packet::bytesToUint16(bytes.data());
//
//    assert(original == converted);
//
//    std::cout << "Uint16 conversion test passed! "
//        << std::hex << original << " -> " << converted << std::dec << "\n\n";
//}
//
//void testInvalidPacket() {
//    std::cout << "Testing invalid packet handling...\n";
//
//    // Слишком короткий пакет
//    std::vector<uint8_t> shortPacket = { 0xAA, 0x01, 0x10 };
//    Packet pkt;
//    bool success = Packet::deserialize(shortPacket, pkt);
//    assert(!success);
//
//    // Неправильный заголовок
//    std::vector<uint8_t> wrongHeader = { 0xBB, 0x01, 0x10, 0x00, 0x00, 0x00 };
//    success = Packet::deserialize(wrongHeader, pkt);
//    assert(!success);
//
//    std::cout << "Invalid packet test passed!\n\n";
//}
//
//void testCRCValidation() {
//    std::cout << "Testing CRC validation...\n";
//
//    Packet pkt;
//    pkt.command_id = 0x10;
//    pkt.payload = { 0x11, 0x22 };
//
//    auto serialized = pkt.serialize();
//
//    // Корректный пакет должен пройти проверку
//    Packet deserialized;
//    bool success = Packet::deserialize(serialized, deserialized);
//    assert(success);
//
//    // Испортим CRC
//    serialized[serialized.size() - 1] ^= 0xFF; // Инвертируем последний байт CRC
//    success = Packet::deserialize(serialized, deserialized);
//    assert(!success); // Должно не пройти проверку
//
//    std::cout << "CRC validation test passed!\n\n";
//}
//
//int main() {
//    std::cout << "=== Starting unit tests ===\n\n";
//
//    try {
//        testCRC();
//        testPacketSerialization();
//        testFloatConversion();
//        testUint16Conversion();
//        testInvalidPacket();
//        testCRCValidation();
//
//        std::cout << "=== All tests passed! ===\n";
//        return 0;
//
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Test failed: " << e.what() << "\n";
//        return 1;
//    }
//    catch (...) {
//        std::cerr << "Unknown test failure\n";
//        return 1;
//    }
//}