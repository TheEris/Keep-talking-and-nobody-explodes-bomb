#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "words.h"

#define I2C_ADDR 0x27  // Define I2C Address for the PCF8574T

#define BACKLIGHT_PIN  3
#define  LED_OFF  1
#define  LED_ON  0

LiquidCrystal_I2C  lcd(I2C_ADDR, 2, 1, 0, 4, 5, 6, 7);

char buff[10];

void setup()
{
  lcd.begin (20, 4);  // 20x4 characters
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(LED_ON);
  lcd.backlight();  //Backlight ON if under program control
}

void loop()
{
  // Reset the display


  for (byte x = 0; x < 28; x++) {
    printWord(x);
    lcd.print(buff);
    delay(1000);

    lcd.clear();
    delay(100);
    lcd.home();
  }
}

void printWord(byte q) {
  for (byte x = 0; x < wordsSize[q] + 1; x++) {
    buff[x] = words[q][x];
  }
  lcd.setCursor(10 - (wordsSize[q] / 2), 0);
}

void getWords() {

}

