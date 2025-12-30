#include "InternalTransports.h"

// ==========================================
// UART TRANSPORT IMPLEMENTATION
// ==========================================

void UartTransport::init() {
    // UART2 on ESP32 defaults to GPIO 16 (RX) and 17 (TX) often, 
    // but we explicitly define them to be safe as per requirement.
    // Baudrate: 115200
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    
    // Debug output on generic Serial
    Serial.println("[UART Transport] Initialized on RX:16, TX:17 at 115200 bps");
}

bool UartTransport::send(const String& data) {
    if (!Serial2) {
        // Try to re-init if not active? Or just fail.
        // Usually Serial2 is always 'true' once begun on ESP32.
        Serial.println("[UART Transport] Error: Serial2 not active");
        return false;
    }

    // Send the data
    Serial2.print(data);
    
    // Wait for transmission to complete
    Serial2.flush();
    
    Serial.print("[UART Transport] Sent: ");
    Serial.println(data);
    return true;
}

// ==========================================
// I2C TRANSPORT IMPLEMENTATION
// ==========================================

I2CTransport::I2CTransport(uint8_t addr) : _slaveAddr(addr) {}

void I2CTransport::init() {
    // I2C on ESP32 defaults to SDA:21, SCL:22
    Wire.begin(21, 22);
    Wire.setClock(100000); // Standard 100kHz
    
    Serial.printf("[I2C Transport] Initialized on SDA:21, SCL:22. Target Addr: 0x%02X\n", _slaveAddr);
}

bool I2CTransport::send(const String& data) {
    // I2C has a hardware buffer limit (usually 32 bytes on Arduino/AVR, 
    // but ESP32 Wire implementation defaults to 128 bytes buffer).
    // However, to be safe and compatible with standard I2C slaves, 
    // sending a huge string in one go might fail if the slave can't handle it.
    // For this implementation, we will send it in one transmission block 
    // assuming the message fits in the buffer (128 bytes on ESP32).
    
    Serial.print("[I2C Transport] Sending: ");
    Serial.println(data);

    Wire.beginTransmission(_slaveAddr);
    Wire.write((const uint8_t*)data.c_str(), data.length());
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
        Serial.println("[I2C Transport] Success (ACK received)");
        return true;
    } else {
        Serial.printf("[I2C Transport] Error: %d\n", error);
        // Error codes: 
        // 1: data too long to fit in transmit buffer
        // 2: received NACK on transmit of address
        // 3: received NACK on transmit of data
        // 4: other error
        return false;
    }
}
