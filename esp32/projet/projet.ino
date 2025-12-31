/**
 * ESP32 Patient Parameters - UART Sender to Wyres-Base
 * 
 * Ce sketch :
 * - Collecte les données patient via clavier 4x4 (ID, Age, Poids)
 * - Affiche sur écran LCD I2C
 * - Envoie les données via UART2 (Serial2) vers Wyres-Base
 * - Attend la réponse de Wyres-Base (LORA_OK / LORA_FAIL)
 * 
 * Connexions ESP32 WROOM-32:
 * 
 * LCD I2C:
 *   - SDA -> GPIO21
 *   - SCL -> GPIO22
 * 
 * Clavier 4x4:
 *   - Row pins: 13, 12, 14, 27
 *   - Col pins: 26, 25, 33, 32
 * 
 * UART2 vers Wyres-Base:
 *   - TX (GPIO17) -> RX Wyres-Base (UART1)
 *   - RX (GPIO16) -> TX Wyres-Base (UART1)
 *   - GND -> GND (commun)
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DHT.h>

// Module capteur température/humidité
#include "Capteur.h"

// Module accéléromètre ADXL345
#include "ADXL345.h"

// ==========================================
// CONFIGURATION UART2
// ==========================================
#define UART2_TX_PIN 17
#define UART2_RX_PIN 16
#define UART2_BAUD 115200
#define RESPONSE_TIMEOUT_MS 30000  // 30 secondes pour attendre LoRaWAN

// ==========================================
// PatientParams Singleton
// ==========================================

class PatientParams {
private:
    PatientParams() : _id("UNKNOWN"), _age(0), _poids(0.0), _pression(0.0), _temperature(0.0), _saturation(0.0) {}
    static PatientParams* _instance;
    
    String _id;
    int _age;
    float _poids;
    float _pression;
    float _temperature;
    float _saturation;
    
    static PatientParams* getInstance() {
        if (_instance == nullptr) {
            _instance = new PatientParams();
        }
        return _instance;
    }

public:
    PatientParams(const PatientParams&) = delete;
    void operator=(const PatientParams&) = delete;

    static void set_id(String id) { getInstance()->_id = id; }
    static void set_age(int age) { getInstance()->_age = age; }
    static void set_poids(float poids) { getInstance()->_poids = poids; }
    static void set_pression(float pression) { getInstance()->_pression = pression; }
    static void set_temperature(float temperature) { getInstance()->_temperature = temperature; }
    static void set_saturation(float saturation) { getInstance()->_saturation = saturation; }
    
    // Format compatible avec WyresReceiver
    static String formatData() {
        PatientParams* p = getInstance();
        String data = "(";
        data += "id : " + p->_id + ", ";
        data += "poids : " + String(p->_poids, 1) + ", ";
        data += "pression : " + String(p->_pression, 1) + ", ";
        data += "temperature : " + String(p->_temperature, 1) + ", ";
        data += "saturation : " + String(p->_saturation, 1);
        data += ")";
        return data;
    }
    
    static void display_parameters() {
        Serial.println("\n========== PATIENT PARAMETERS ==========");
        Serial.println(formatData());
        Serial.println("=========================================\n");
    }
    
    // Envoyer via UART2 vers Wyres-Base
    static void send_to_wyres() {
        String payload = formatData();
        
        Serial.println("[UART2] Envoi vers Wyres-Base...");
        Serial.println("[UART2] Payload: " + payload);
        
        // Envoyer les données
        Serial2.println(payload);
        Serial2.flush();
        
        Serial.println("[UART2] Données envoyées. Attente réponse...");
    }
    
    // Attendre réponse de Wyres-Base
    static String wait_response(unsigned long timeout_ms) {
        String response = "";
        unsigned long start = millis();
        
        while (millis() - start < timeout_ms) {
            if (Serial2.available()) {
                char c = Serial2.read();
                if (c == '\n') {
                    break;
                } else if (c != '\r') {
                    response += c;
                }
            }
            delay(10);
        }
        
        return response;
    }
    
    // Reset pour nouvelle saisie
    static void reset() {
        PatientParams* p = getInstance();
        p->_id = "UNKNOWN";
        p->_age = 0;
        p->_poids = 0.0;
        p->_pression = 0.0;
        p->_temperature = 0.0;
        p->_saturation = 0.0;
    }
};

PatientParams* PatientParams::_instance = nullptr;

// ==========================================
// LCD Setup
// ==========================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcdInit() {
    Wire.begin(21, 22);
    lcd.init();
    lcd.backlight();
}

void lcdShowID(const String& id) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID (4 car):");
    lcd.setCursor(0, 1);
    lcd.print(id);
}

void lcdShowAge(const String& age) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Age (2 car):");
    lcd.setCursor(0, 1);
    lcd.print(age);
}

void lcdShowPoids(const String& poids) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Poids XX.XX:");
    lcd.setCursor(0, 1);
    lcd.print(poids);
}

void lcdShowResult(const String& id, const String& age, const String& poids) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID:");
    lcd.print(id);
    lcd.setCursor(0, 1);
    lcd.print("A:");
    lcd.print(age);
    lcd.print(" P:");
    lcd.print(poids);
}

void lcdShowStatus(const String& line1, const String& line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

// ==========================================
// Keypad Setup
// ==========================================

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char lireTouche() {
    return keypad.getKey();
}

bool estChiffre(char c) {
    return (c >= '0' && c <= '9');
}

// ==========================================
// State Machine
// ==========================================

enum Etat { SAISIE_ID, SAISIE_AGE, SAISIE_POIDS, LECTURE_CAPTEUR, LECTURE_ACCEL, ENVOI, ATTENTE_REPONSE, TERMINE };
Etat etat = SAISIE_ID;

String id = "";
String age = "";
String poids = "";

// ==========================================
// Arduino Setup & Loop
// ==========================================

void setup() {
    // Debug USB
    Serial.begin(115200);
    
    // UART2 vers Wyres-Base
    Serial2.begin(UART2_BAUD, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);
    
    Serial.println("\n=== ESP32 Patient Parameters ===");
    Serial.println("UART2 initialisé sur TX:17, RX:16 @ 115200");
    Serial.println("Entrez ID (4 car), Age (2 chiffres), Poids (XX.XX)");
    Serial.println("Touche D = Valider, Touche * = Effacer\n");
    
    lcdInit();
    
    // Initialisation capteur DHT
    Capteur::init();
    
    // Initialisation accéléromètre ADXL345
    ADXL345::init();
    
    lcdShowID(id);
}

void loop() {
    char key = lireTouche();
    
    switch (etat) {
        case SAISIE_ID:
        case SAISIE_AGE:
        case SAISIE_POIDS:
            handleSaisie(key);
            break;
            
        case LECTURE_CAPTEUR:
            handleLectureCapteur();
            break;
            
        case LECTURE_ACCEL:
            handleLectureAccel();
            break;
            
        case ENVOI:
            handleEnvoi();
            break;
            
        case ATTENTE_REPONSE:
            handleAttenteReponse();
            break;
            
        case TERMINE:
            handleTermine(key);
            break;
    }
}

void handleSaisie(char key) {
    if (!key) return;
    
    // Effacer avec *
    if (key == '*') {
        if (etat == SAISIE_ID) id = "";
        if (etat == SAISIE_AGE) age = "";
        if (etat == SAISIE_POIDS) poids = "";
    }
    
    // Valider avec D
    if (key == 'D') {
        if (etat == SAISIE_ID && id.length() == 4) {
            etat = SAISIE_AGE;
            lcdShowAge(age);
        }
        else if (etat == SAISIE_AGE && age.length() == 2) {
            etat = SAISIE_POIDS;
            lcdShowPoids(poids);
        }
        else if (etat == SAISIE_POIDS && poids.length() == 5) {
            etat = LECTURE_CAPTEUR;
        }
    } 
    else if (key != '*') {
        if (etat == SAISIE_ID && id.length() < 4) {
            id += key;
            lcdShowID(id);
        }
        else if (etat == SAISIE_AGE && age.length() < 2 && estChiffre(key)) {
            age += key;
            lcdShowAge(age);
        }
        else if (etat == SAISIE_POIDS && poids.length() < 5) {
            if (poids.length() == 2) poids += '.';
            if (estChiffre(key)) {
                poids += key;
                lcdShowPoids(poids);
            }
        }
    }
}

unsigned long envoi_start = 0;

void handleLectureCapteur() {
    lcdShowStatus("Lecture...", "Capteur DHT");
    
    Serial.println("[Capteur] Lecture en cours...");
    
    if (Capteur::read()) {
        // Afficher les valeurs lues
        Capteur::displayOnLCD(lcd);
        delay(2000);  // Afficher 2 secondes
        
        // Mapper sur PatientParams:
        // - température -> temperature
        // - humidité -> saturation (mappé comme demandé)
        PatientParams::set_temperature(Capteur::getTemperature());
        PatientParams::set_saturation(Capteur::getHumidity());  // Humidité -> Saturation
        
        Serial.println("[Capteur] Valeurs mappées: Temp=" + String(Capteur::getTemperature()) + 
                       ", Humid/Sat=" + String(Capteur::getHumidity()));
        
        etat = LECTURE_ACCEL;  // Passer à la lecture accéléromètre
    } else {
        lcdShowStatus("Erreur DHT!", "Appuyez #");
        Serial.println("[Capteur] Échec lecture, appuyez # pour réessayer");
        etat = TERMINE;  // Permettre de recommencer
    }
}

void handleLectureAccel() {
    lcdShowStatus("Lecture...", "Accelerometre");
    
    Serial.println("[ADXL345] Lecture en cours...");
    
    if (ADXL345::read()) {
        // Afficher sur LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Accel Z:");
        lcd.print(ADXL345::getZ(), 2);
        lcd.print("g");
        lcd.setCursor(0, 1);
        lcd.print("Mag:");
        lcd.print(ADXL345::getMagnitude(), 2);
        delay(2000);
        
        // Mapper l'accélération Z sur pression
        // (valeur Z en g, ~1.0 au repos, varie avec le mouvement)
        PatientParams::set_pression(ADXL345::getZ());
        
        Serial.println("[ADXL345] Z mappé sur pression: " + String(ADXL345::getZ()));
        
        etat = ENVOI;
    } else {
        lcdShowStatus("Erreur ADXL!", "Appuyez #");
        Serial.println("[ADXL345] Échec lecture");
        etat = TERMINE;
    }
}

void handleEnvoi() {
    lcdShowStatus("Envoi...", "Vers Wyres-Base");
    
    // Configurer les paramètres
    PatientParams::set_id(id);
    PatientParams::set_age(age.toInt());
    PatientParams::set_poids(poids.toFloat());
    
    // Afficher et envoyer
    PatientParams::display_parameters();
    PatientParams::send_to_wyres();
    
    envoi_start = millis();
    etat = ATTENTE_REPONSE;
    
    lcdShowStatus("Attente LoRa...", "Patience...");
}

void handleAttenteReponse() {
    // Vérifier si réponse disponible
    if (Serial2.available()) {
        String response = PatientParams::wait_response(5000);
        
        Serial.println("[UART2] Réponse reçue: " + response);
        
        if (response.indexOf("LORA_OK") >= 0) {
            lcdShowStatus("SUCCES!", "Donnees envoyees");
            Serial.println("✅ Données envoyées via LoRaWAN!");
        } else if (response.indexOf("LORA_FAIL") >= 0) {
            lcdShowStatus("ECHEC!", "LoRa erreur");
            Serial.println("❌ Échec envoi LoRaWAN");
        } else {
            lcdShowStatus("Reponse:", response.substring(0, 16));
        }
        
        etat = TERMINE;
    }
    
    // Timeout après 30 secondes
    if (millis() - envoi_start > RESPONSE_TIMEOUT_MS) {
        lcdShowStatus("TIMEOUT!", "Pas de reponse");
        Serial.println("⚠️ Timeout: pas de réponse de Wyres-Base");
        etat = TERMINE;
    }
}

void handleTermine(char key) {
    // Appuyer sur # pour recommencer
    if (key == '#') {
        PatientParams::reset();
        id = "";
        age = "";
        poids = "";
        etat = SAISIE_ID;
        lcdShowID(id);
        Serial.println("\n=== Nouvelle saisie ===\n");
    }
}
