#include "lcd_ui.h"
#include "input.h"

enum Etat { ID, AGE, POIDS, FIN };
Etat etat = ID;

String id = "";
String age = "";
String poids = "";

void setup() {
  lcdInit();
  lcdShowID(id);
}

void loop() {
  char key = lireTouche();
  if (!key) return;

  if (key == '*') {
    if (etat == ID) id = "";
    if (etat == AGE) age = "";
    if (etat == POIDS) poids = "";
  }

  if (key == 'D') {
    if (etat == ID && id.length() == 4) etat = AGE;
    else if (etat == AGE && age.length() == 2) etat = POIDS;
    else if (etat == POIDS && poids.length() == 5) etat = FIN;
  } 
  else {
    if (etat == ID && id.length() < 4) id += key;
    else if (etat == AGE && age.length() < 2 && estChiffre(key)) age += key;
    else if (etat == POIDS && poids.length() < 5) {
      if (poids.length() == 2) poids += '.';
      if (estChiffre(key)) poids += key;
    }
  }

  if (etat == ID) lcdShowID(id);
  else if (etat == AGE) lcdShowAge(age);
  else if (etat == POIDS) lcdShowPoids(poids);
  else lcdShowResult(id, age, poids);
}
