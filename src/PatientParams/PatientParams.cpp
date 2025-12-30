#include "PatientParams.h"

// Initialize static pointer to null
PatientParams* PatientParams::_instance = nullptr;

// Constructor (Private)
PatientParams::PatientParams() {
    // Initialize with safe defaults
    _id = "UNKNOWN";
    _poids = 0.0;
    _pression = 0.0;
    _temperature = 0.0;
    _saturation = 0.0;
}

// Get Instance (Lazy Initialization)
PatientParams* PatientParams::getInstance() {
    if (_instance == nullptr) {
        _instance = new PatientParams();
    }
    return _instance;
}

// ==========================================
// SETTERS IMPLEMENTATION
// ==========================================

void PatientParams::set_id(String id) {
    getInstance()->_id = id;
}

void PatientParams::set_poids(float poids) {
    getInstance()->_poids = poids;
}

void PatientParams::set_pression(float pression) {
    getInstance()->_pression = pression;
}

void PatientParams::set_temperature(float temperature) {
    getInstance()->_temperature = temperature;
}

void PatientParams::set_saturation(float saturation) {
    getInstance()->_saturation = saturation;
}

// ==========================================
// INTERNAL HELPERS
// ==========================================

String PatientParams::_formatData() {
    // Format: (id : <valeur>, poids : <valeur>, pression : <valeur>, température : <valeur>, saturation : <valeur>)
    String data = "(";
    data += "id : " + _id + ", ";
    data += "poids : " + String(_poids, 1) + ", ";
    data += "pression : " + String(_pression, 1) + ", ";
    data += "température : " + String(_temperature, 1) + ", ";
    data += "saturation : " + String(_saturation, 1);
    data += ")";
    return data;
}

// ==========================================
// SENDING IMPLEMENTATION
// ==========================================

void PatientParams::send_via_uart() {
    // 1. Create Transport
    UartTransport uart;
    uart.init(); // Ensure serial is started
    
    // 2. Format Data
    String payload = getInstance()->_formatData();
    
    // 3. Send
    uart.send(payload);
}

String PatientParams::wait_response_uart(unsigned long timeout_ms) {
    UartTransport uart;
    uart.init();
    return uart.receive(timeout_ms);
}

void PatientParams::send_via_i2c() {
    // 1. Create Transport
    // Note: In a real heavy app, we might want to keep the transport instance alive
    // to avoid re-initializing Wire every time if it was costly, BUT
    // Wire.begin() is safe to call multiple times (it just re-sets registers).
    // For simplicity and statelessness of the API, we create it on the fly.
    I2CTransport i2c;
    i2c.init(); 
    
    // 2. Format Data
    String payload = getInstance()->_formatData();
    
    // 3. Send
    i2c.send(payload);
}
