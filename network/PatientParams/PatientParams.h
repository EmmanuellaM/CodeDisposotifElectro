#ifndef PATIENT_PARAMS_H
#define PATIENT_PARAMS_H

#include <Arduino.h>
#include "InternalTransports.h"

class PatientParams {
private:
    // Private Constructor for Singleton
    PatientParams();
    
    // The single instance
    static PatientParams* _instance;
    
    // Internal data fields
    String _id;
    float _poids;
    float _pression;
    float _temperature;
    float _saturation;
    
    // Internal helper to get/create instance
    static PatientParams* getInstance();
    
    // Internal helper to format data string
    String _formatData();

public:
    // Delete copy constructor and assignment operator
    PatientParams(const PatientParams&) = delete;
    void operator=(const PatientParams&) = delete;

    // ==========================================
    // DATA SETTERS (Static API)
    // ==========================================
    static void set_id(String id);
    static void set_poids(float poids);
    static void set_pression(float pression);
    static void set_temperature(float temperature);
    static void set_saturation(float saturation);
    
    // ==========================================
    // SENDING METHODS (Static API)
    // ==========================================
    
    // Send using UART (Pins 16/17, 115200 baud)
    static void send_via_uart();
    
    // Send using I2C (Pins 21/22, Addr 8)
    static void send_via_i2c();
};

#endif // PATIENT_PARAMS_H
