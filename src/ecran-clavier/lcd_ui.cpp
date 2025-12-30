#include "lcd_ui.h"
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcdInit() {
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
}

void lcdShowID(const String& id) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ID (4 car):");
  lcd.setCursor(0,1);
  lcd.print(id);
}

void lcdShowAge(const String& age) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Age (2 car):");
  lcd.setCursor(0,1);
  lcd.print(age);
}

void lcdShowPoids(const String& poids) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Poids XX.XX:");
  lcd.setCursor(0,1);
  lcd.print(poids);
}

void lcdShowResult(const String& id, const String& age, const String& poids) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ID:");
  lcd.print(id);
  lcd.setCursor(0,1);
  lcd.print("A:");
  lcd.print(age);
  lcd.print(" P:");
  lcd.print(poids);
}
