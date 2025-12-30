# PatientParams Library Documentation

## Description
**PatientParams** est une librairie C++ pour ESP32 conçue pour simplifier l'envoi de données physiologiques (paramètres vitaux) vers un système tiers.

Elle utilise le pattern **Singleton** pour garantir une gestion centralisée des données et offre une abstraction complète du matériel. L'utilisateur n'a pas besoin de configurer les broches ou les protocoles : tout est géré automatiquement.

## Configuration Matérielle (Fixe)
La librairie utilise des broches prédéfinies pour l'ESP32. Assurez-vous que votre câblage correspond à ce standard :

| Protocole | Broche ESP32 | Fonction |
|-----------|--------------|----------|
| **UART2** | **GPIO 16**  | RX (Réception) |
|           | **GPIO 17**  | TX (Transmission) |
| **I2C**   | **GPIO 21**  | SDA (Données) |
|           | **GPIO 22**  | SCL (Horloge) |

*Note : Les vitesses sont fixées à **115200 bauds** pour l'UART et **100kHz** pour l'I2C.*

##  Installation
1. Copier le dossier `PatientParams` dans le dossier `src` de votre projet Arduino/PlatformIO.
2. Inclure le header principal dans votre sketch :
   ```cpp
   #include "src/PatientParams/PatientParams.h"
   ```

##  Référence API

Toutes les méthodes sont **statiques**, vous n'avez pas besoin d'instancier la classe.

### 1. Configuration des Données

Ces méthodes permettent de définir les valeurs qui seront envoyées.

| Méthode | Paramètre | Description |
|---------|-----------|-------------|
| `PatientParams::set_id(String id)` | `id` (e.g. "PATIENT_01") | Identifiant unique du patient ou de la session. |
| `PatientParams::set_poids(float val)` | `val` (kg) | Poids du patient. |
| `PatientParams::set_pression(float val)` | `val` (cmHg) | Pression artérielle systolique/diastolique (format libre). |
| `PatientParams::set_temperature(float val)`| `val` (°C) | Température corporelle. |
| `PatientParams::set_saturation(float val)` | `val` (%) | Saturation en oxygène (SpO2). |

### 2. Envoi des Données

Ces méthodes formatent les données actuelles et les envoient via le canal choisi.

#### `PatientParams::send_via_uart()`
*   Initialise le port série UART2 (s'il ne l'est pas déjà).
*   Envoie la chaîne formatée sur les broches 16/17.
*   Bloque jusqu'à la fin de la transmission (flush).

#### `PatientParams::send_via_i2c()`
*   Initialise le bus I2C (Wire) sur les broches 21/22.
*   Envoie la chaîne formatée à l'adresse esclave **8** (0x08).
*   *Attention : La taille du message est limitée par le buffer I2C (généralement 128 octets sur ESP32).*

## Format des Données (Protocole)
Les données sont envoyées sous forme d'une chaîne de caractères ASCII (String) unique, formatée comme suit :

```text
(id : <ID>, poids : <POIDS>, pression : <PRESSION>, température : <TEMP>, saturation : <SAT>)
```

**Exemple de sortie réelle :**
```text
(id : TEST_PATIENT_01, poids : 70.5, pression : 12.5, température : 37.0, saturation : 99.0)
```

## Exemple d'Utilisation

```cpp
#include <Arduino.h>
#include "src/PatientParams/PatientParams.h"

void setup() {
  Serial.begin(115200); // Pour le debug USB uniquement
}

void loop() {
  // 1. Remplir les données
  PatientParams::set_id("MME_DUPONT");
  PatientParams::set_poids(64.2);
  PatientParams::set_temperature(37.5);
  // Les autres valeurs restent à 0.0 par défaut si non définies

  // 2. Envoyer (choisir une methode)
  PatientParams::send_via_uart(); 
  // ou
  // PatientParams::send_via_i2c();

  delay(10000); // Attendre 10 secondes
}
```
