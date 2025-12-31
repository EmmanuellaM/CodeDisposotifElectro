#ifndef INTERNAL_TRANSPORTS_H
#define INTERNAL_TRANSPORTS_H

#include <Arduino.h>
#include <Wire.h>

// Abstract Strategy Interface
class ICommsStrategy {
public:
    virtual ~ICommsStrategy() {}
    virtual void init() = 0;
    virtual bool send(const String& data) = 0;
    virtual String receive(unsigned long timeout_ms) = 0; // NEW: Receive with timeout
};

// Concrete UART Transport (Hardcoded Pins 16/17, Baud 115200)
class UartTransport : public ICommsStrategy {
public:
    void init() override;
    bool send(const String& data) override;
    String receive(unsigned long timeout_ms) override;
};

// Concrete I2C Transport (Hardcoded Pins 21/22, Addr 8)
class I2CTransport : public ICommsStrategy {
private:
    uint8_t _slaveAddr;
public:
    I2CTransport(uint8_t addr = 8); // Default address 8
    void init() override;
    bool send(const String& data) override;
    String receive(unsigned long timeout_ms) override { return ""; } // Not implemented for I2C master yet
};

#endif // INTERNAL_TRANSPORTS_H
