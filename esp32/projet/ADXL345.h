/**
 * ADXL345.h - Module accéléromètre I2C pour ESP32
 * 
 * Câblage (second bus I2C):
 *   - VCC  → 3.3V
 *   - GND  → GND
 *   - SDA  → GPIO 19
 *   - SCL  → GPIO 18
 *   - CS   → 3.3V (mode I2C)
 *   - SDO  → GND (adresse 0x53)
 */

#ifndef ADXL345_H
#define ADXL345_H

#include <Wire.h>

// Configuration I2C secondaire (GPIO 18/19)
#ifndef ADXL_SDA
#define ADXL_SDA 19
#endif

#ifndef ADXL_SCL
#define ADXL_SCL 18
#endif

// Adresse I2C (SDO à GND = 0x53)
#define ADXL345_ADDR 0x53

// Registres
#define ADXL345_REG_DEVID       0x00
#define ADXL345_REG_POWER_CTL   0x2D
#define ADXL345_REG_DATA_FORMAT 0x31
#define ADXL345_REG_DATAX0      0x32

class ADXL345 {
private:
    static TwoWire* _wire;
    static bool _initialized;
    static float _gX, _gY, _gZ;

    static void writeReg(uint8_t reg, uint8_t val) {
        _wire->beginTransmission(ADXL345_ADDR);
        _wire->write(reg);
        _wire->write(val);
        _wire->endTransmission();
    }

    static uint8_t readReg(uint8_t reg) {
        _wire->beginTransmission(ADXL345_ADDR);
        _wire->write(reg);
        _wire->endTransmission();
        _wire->requestFrom(ADXL345_ADDR, 1);
        return _wire->read();
    }

public:
    /**
     * @brief Initialise l'ADXL345 sur I2C secondaire
     * @return true si succès
     */
    static bool init() {
        if (_initialized) return true;
        
        _wire = new TwoWire(1);  // Second bus I2C
        _wire->begin(ADXL_SDA, ADXL_SCL, 100000);
        
        // Vérifier Device ID
        uint8_t devId = readReg(ADXL345_REG_DEVID);
        Serial.print("[ADXL345] Device ID: 0x");
        Serial.println(devId, HEX);
        
        if (devId != 0xE5) {
            Serial.println("[ADXL345] ERREUR: Capteur non trouvé!");
            return false;
        }
        
        // Configurer (+/- 2g, pleine résolution)
        writeReg(ADXL345_REG_DATA_FORMAT, 0x08);
        writeReg(ADXL345_REG_POWER_CTL, 0x08);
        
        _initialized = true;
        Serial.println("[ADXL345] Initialisé sur GPIO 18/19");
        return true;
    }

    /**
     * @brief Lit les valeurs d'accélération
     * @return true si lecture réussie
     */
    static bool read() {
        if (!_initialized) {
            if (!init()) return false;
        }
        
        uint8_t buffer[6];
        _wire->beginTransmission(ADXL345_ADDR);
        _wire->write(ADXL345_REG_DATAX0);
        _wire->endTransmission();
        _wire->requestFrom(ADXL345_ADDR, 6);
        
        for (int i = 0; i < 6; i++) {
            buffer[i] = _wire->read();
        }
        
        int16_t rawX = (int16_t)((buffer[1] << 8) | buffer[0]);
        int16_t rawY = (int16_t)((buffer[3] << 8) | buffer[2]);
        int16_t rawZ = (int16_t)((buffer[5] << 8) | buffer[4]);
        
        _gX = rawX / 256.0;
        _gY = rawY / 256.0;
        _gZ = rawZ / 256.0;
        
        Serial.print("[ADXL345] X:");
        Serial.print(_gX, 2);
        Serial.print(" Y:");
        Serial.print(_gY, 2);
        Serial.print(" Z:");
        Serial.println(_gZ, 2);
        
        return true;
    }

    static float getX() { return _gX; }
    static float getY() { return _gY; }
    static float getZ() { return _gZ; }
    
    /**
     * @brief Calcule la magnitude totale (norme du vecteur)
     */
    static float getMagnitude() {
        return sqrt(_gX*_gX + _gY*_gY + _gZ*_gZ);
    }
};

// Initialisation membres statiques
TwoWire* ADXL345::_wire = nullptr;
bool ADXL345::_initialized = false;
float ADXL345::_gX = 0.0;
float ADXL345::_gY = 0.0;
float ADXL345::_gZ = 0.0;

#endif // ADXL345_H
