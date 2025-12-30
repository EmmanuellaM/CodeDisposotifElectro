#ifndef LCD_UI_H
#define LCD_UI_H

#include <LiquidCrystal_I2C.h>

void lcdInit();
void lcdShowID(const String& id);
void lcdShowAge(const String& age);
void lcdShowPoids(const String& poids);
void lcdShowResult(const String& id, const String& age, const String& poids);

#endif
