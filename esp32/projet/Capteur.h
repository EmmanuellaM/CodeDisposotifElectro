/**
 * Capteur.h - Module capteur DHT11/DHT22 pour ESP32
 * 
 * Lit température et humidité depuis un capteur DHT.
 * - Température -> PatientParams::temperature
 * - Humidité -> PatientParams::saturation (mappé)
 */

#ifndef CAPTEUR_H
#define CAPTEUR_H

#include <DHT.h>

// Configuration par défaut (peut être modifiée avant l'include)
#ifndef DHT_PIN
#define DHT_PIN 4
#endif

#ifndef DHT_TYPE
#define DHT_TYPE DHT11  // ou DHT22
#endif

class Capteur {
private:
    static DHT* _dht;
    static bool _initialized;
    static float _lastTemperature;
    static float _lastHumidity;

public:
    /**
     * @brief Initialise le capteur DHT
     */
    static void init() {
        if (!_initialized) {
            _dht = new DHT(DHT_PIN, DHT_TYPE);
            _dht->begin();
            _initialized = true;
            Serial.println("[Capteur] DHT initialisé sur GPIO " + String(DHT_PIN));
        }
    }
    
    /**
     * @brief Lit les valeurs du capteur
     * @return true si lecture réussie, false sinon
     */
    static bool read() {
        if (!_initialized) {
            init();
        }
        
        float h = _dht->readHumidity();
        float t = _dht->readTemperature();
        
        if (isnan(h) || isnan(t)) {
            Serial.println("[Capteur] Erreur de lecture!");
            return false;
        }
        
        _lastTemperature = t;
        _lastHumidity = h;
        
        Serial.print("[Capteur] T: ");
        Serial.print(t);
        Serial.print("°C | H: ");
        Serial.print(h);
        Serial.println("%");
        
        return true;
    }
    
    /**
     * @brief Retourne la dernière température lue
     */
    static float getTemperature() {
        return _lastTemperature;
    }
    
    /**
     * @brief Retourne la dernière humidité lue
     */
    static float getHumidity() {
        return _lastHumidity;
    }
    
    /**
     * @brief Affiche les valeurs sur LCD
     * @param lcd Référence vers l'objet LCD
     */
    static void displayOnLCD(LiquidCrystal_I2C& lcd) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T:");
        lcd.print(_lastTemperature, 1);
        lcd.print((char)223);  // Symbole degré
        lcd.print("C");
        
        lcd.setCursor(0, 1);
        lcd.print("H:");
        lcd.print(_lastHumidity, 1);
        lcd.print("%");
    }
};

// Initialisation des membres statiques
DHT* Capteur::_dht = nullptr;
bool Capteur::_initialized = false;
float Capteur::_lastTemperature = 0.0;
float Capteur::_lastHumidity = 0.0;

#endif // CAPTEUR_H
