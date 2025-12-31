/**
 * Test ADXL345 - Accéléromètre I2C sur ESP32
 * 
 * Câblage (second bus I2C sur GPIO 18/19):
 *   - VCC  → 3.3V
 *   - GND  → GND
 *   - SDA  → GPIO 19
 *   - SCL  → GPIO 18
 *   - CS   → 3.3V (mode I2C)
 *   - SDO  → GND (adresse 0x53)
 */

#include <Wire.h>

// Configuration I2C secondaire
#define ADXL_SDA 19
#define ADXL_SCL 18

// Adresse I2C de l'ADXL345 (SDO à GND = 0x53, SDO à VCC = 0x1D)
#define ADXL345_ADDR 0x53

// Registres ADXL345
#define ADXL345_REG_DEVID       0x00  // Device ID (doit retourner 0xE5)
#define ADXL345_REG_POWER_CTL   0x2D  // Power control
#define ADXL345_REG_DATA_FORMAT 0x31  // Format des données
#define ADXL345_REG_DATAX0      0x32  // Début des données X (6 bytes)

// Instance Wire pour le second bus I2C
TwoWire WireADXL = TwoWire(1);  // Bus I2C numéro 1

// Variables pour stocker les valeurs
int16_t accelX, accelY, accelZ;
float gX, gY, gZ;

// Écrire un byte dans un registre
void writeRegister(uint8_t reg, uint8_t value) {
    WireADXL.beginTransmission(ADXL345_ADDR);
    WireADXL.write(reg);
    WireADXL.write(value);
    WireADXL.endTransmission();
}

// Lire un byte depuis un registre
uint8_t readRegister(uint8_t reg) {
    WireADXL.beginTransmission(ADXL345_ADDR);
    WireADXL.write(reg);
    WireADXL.endTransmission();
    WireADXL.requestFrom(ADXL345_ADDR, 1);
    return WireADXL.read();
}

// Lire plusieurs bytes
void readRegisters(uint8_t reg, uint8_t count, uint8_t* dest) {
    WireADXL.beginTransmission(ADXL345_ADDR);
    WireADXL.write(reg);
    WireADXL.endTransmission();
    WireADXL.requestFrom(ADXL345_ADDR, count);
    for (uint8_t i = 0; i < count; i++) {
        dest[i] = WireADXL.read();
    }
}

// Initialiser l'ADXL345
bool initADXL345() {
    // Vérifier Device ID
    uint8_t devId = readRegister(ADXL345_REG_DEVID);
    Serial.print("[ADXL345] Device ID: 0x");
    Serial.println(devId, HEX);
    
    if (devId != 0xE5) {
        Serial.println("[ADXL345] ERREUR: Device ID incorrect!");
        return false;
    }
    
    // Configurer le format des données (+/- 2g, pleine résolution)
    writeRegister(ADXL345_REG_DATA_FORMAT, 0x08);
    
    // Activer les mesures
    writeRegister(ADXL345_REG_POWER_CTL, 0x08);
    
    Serial.println("[ADXL345] Initialisé avec succès!");
    return true;
}

// Lire les valeurs d'accélération
void readAcceleration() {
    uint8_t buffer[6];
    readRegisters(ADXL345_REG_DATAX0, 6, buffer);
    
    // Combiner les bytes (little-endian)
    accelX = (int16_t)((buffer[1] << 8) | buffer[0]);
    accelY = (int16_t)((buffer[3] << 8) | buffer[2]);
    accelZ = (int16_t)((buffer[5] << 8) | buffer[4]);
    
    // Convertir en g (pour +/- 2g, sensibilité = 256 LSB/g)
    gX = accelX / 256.0;
    gY = accelY / 256.0;
    gZ = accelZ / 256.0;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== Test ADXL345 Accéléromètre ===");
    Serial.println("I2C sur GPIO 18 (SCL) et GPIO 19 (SDA)");
    
    // Initialiser le second bus I2C
    WireADXL.begin(ADXL_SDA, ADXL_SCL, 100000);
    
    // Scanner I2C pour trouver les devices
    Serial.println("\n[I2C] Scan des adresses...");
    int devicesFound = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        WireADXL.beginTransmission(addr);
        if (WireADXL.endTransmission() == 0) {
            Serial.print("  Device trouvé à 0x");
            Serial.println(addr, HEX);
            devicesFound++;
        }
    }
    Serial.print("[I2C] ");
    Serial.print(devicesFound);
    Serial.println(" device(s) trouvé(s)\n");
    
    // Initialiser ADXL345
    if (!initADXL345()) {
        Serial.println("ERREUR: Impossible d'initialiser ADXL345!");
        Serial.println("Vérifiez le câblage et l'adresse I2C.");
        while (1) delay(1000);
    }
    
    Serial.println("\n--- Lecture accélération ---");
    Serial.println("  X (g)   |   Y (g)   |   Z (g)");
    Serial.println("----------|-----------|----------");
}

void loop() {
    readAcceleration();
    
    // Afficher les valeurs
    Serial.print("  ");
    Serial.print(gX, 2);
    Serial.print("    |   ");
    Serial.print(gY, 2);
    Serial.print("    |   ");
    Serial.println(gZ, 2);
    
    delay(500);
}
