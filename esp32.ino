/*
 * ============================================================================
 * ESP32 - Test Envoi UART avec Données en Dur
 * ============================================================================
 * 
 * Ce programme utilise la librairie PatientParams pour envoyer 
 * des valeurs fixes via UART2 (GPIO 16/17).
 * 
 * ============================================================================
 */

#include <Arduino.h>
#include "src/PatientParams/PatientParams.h"

void setup() {
  // Debug USB
  Serial.begin(115200);
  delay(1000);
  Serial.println(">>> Démarrage du test UART <<<");
}

void loop() {
  // 1. Définition des données en dur (Hardcoded)
  Serial.println("Configuration des données...");
  
  PatientParams::set_id("TEST_PATIENT_01");
  PatientParams::set_poids(70.5);
  PatientParams::set_pression(12.5);
  PatientParams::set_temperature(37.0);
  PatientParams::set_saturation(99.0);

// 2. Envoi via UART (Pins 16/17)
  Serial.println("Envoi en cours via UART...");
  PatientParams::send_via_uart();
  
  // 3. Attente de la réception
  Serial.println("Attente de la réponse de Wyres-Base...");
  String response = PatientParams::wait_response_uart(5000); // 5 sec timeout
  
  if (response.length() > 0) {
      Serial.print(">>> RÉPONSE REÇUE : ");
      Serial.println(response);
  } else {
      Serial.println(">>> TIMEOUT : Aucune réponse reçue.");
  }
  
  Serial.println("Attente avant prochain cycle...\n");
  
  // 4. Attente
  delay(5000);
}
