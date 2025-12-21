# Dispositif Médical Electro

## Description

Ce projet est un dispositif embarqué qui collecte les données médicales d'un patient et les envoie via LoRaWAN vers une application distante.

## Comment ça marche ?

1. Le responsable enregistre l'ID et le poids du patient
2. Deux capteurs médicaux récupèrent automatiquement les données du patient
3. Les données sont envoyées via LoRaWAN vers l'application

## Structure du projet

```
medical-device-lora/
├── include/              # Fichiers .h (déclarations)
│   ├── config.h         # Configuration générale
│   ├── patient.h        # Gestion patient (ID + poids)
│   ├── sensors.h        # Gestion des 2 capteurs
│   └── lora_comm.h      # Communication LoRaWAN
│
├── src/                 # Fichiers .cpp (implémentation)
│   ├── main.cpp         # Programme principal
│   ├── patient.cpp
│   ├── sensors.cpp
│   └── lora_comm.cpp
│
├── lib/                 # Bibliothèques des capteurs
├── test/                # Tests unitaires
└── docs/                # Documentation technique
```

## Installation

### 1. Cloner le projet
```bash
git clone https://github.com/votre-organisation/medical-device-lora.git
cd CodeDispositifElectro
```

### 2. Configurer les identifiants LoRaWAN

Créer un fichier `include/secrets.h` avec vos clés :
```cpp
#define DEVEUI "0000000000000000"
#define APPEUI "0000000000000000"
#define APPKEY "00000000000000000000000000000000"
```

⚠️ **Ne jamais partager ce fichier sur Git !**

### 3. Compiler et téléverser
```bash
pio run --target upload
```

## Utilisation

1. Allumer le dispositif
2. Scanner l'ID du patient
3. Mesurer le poids
4. Les capteurs prennent les mesures automatiquement
5. Les données sont envoyées via LoRaWAN

## Travailler en équipe

### Créer une nouvelle fonctionnalité
```bash
git checkout -b feature/ma-fonctionnalite
# Faire vos modifications
git add .
git commit -m "Description de ce que vous avez fait"
git push origin feature/ma-fonctionnalite
```

### Règles simples
- Une branche par fonctionnalité
- Des commits clairs et réguliers
- Tester avant de pousser
- Faire une Pull Request pour fusionner

## Aide

### Problème de connexion LoRaWAN ?
- Vérifier les clés dans `secrets.h`
- Vérifier la couverture réseau

### Problème avec les capteurs ?
- Vérifier les câbles
- Regarder les logs dans le moniteur série

