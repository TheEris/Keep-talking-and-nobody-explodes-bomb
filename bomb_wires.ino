#include <Wire.h>
#include "def.h"

const byte pins[6] = {4, 5, 6, 7, 8, 9};    //  input pins for wires, pullups
byte In[6];                                 //  holds the pin status
byte serBuf[10];                            //  temp buffer for serial

byte wireCount = 0;                         //  number of wires
byte wirePosition[6];                       //  where are they
byte wireColor[6];                          //  wire colors
byte cutMask = 0;                           //  position where cut the wire


void setup() {
  Serial.begin(9600);
  while (!Serial);

  for (byte x = 0; x < 6; x++) {
    pinMode(pins[x], INPUT_PULLUP);
  }

  scanInputs();
  Info();

  while (!Serial.available()) {
    delay(500);
  }

  getColors();
  getCutMask();
}

void loop() {
  //getWires();
  //Check();
  delay(100);
}

void scanInputs() {
  for (byte x = 0; x < 6; x++) {
    if (!digitalRead(pins[x])) {
      wirePosition[wireCount] = x + 1;
      wireCount++;
    }
  }

  if (wireCount < 3) {
    Serial.println(F("Please connect atleast 3 wires to use this module"));
    Serial.println(F("Restart this module to try again"));
    while (true) {}
  }
}

void Info() {
  Serial.println(F("Keep Talking and nobody explodes - simple Wires module"));
  Serial.print(F("Detected: "));
  Serial.print(wireCount);
  Serial.println(F(" wires"));
  Serial.println(F("-------------------------------------------------------"));
  Serial.println(F("Please type in thier color with int value from table below,"));
  Serial.println(F("in format '121554'."));
  Serial.println(F("1 = RED"));
  Serial.println(F("2 = BLUE"));
  Serial.println(F("3 = YELLOW"));
  Serial.println(F("4 = WHITE"));
  Serial.println(F("5 = BLACK"));
  Serial.println(F("-------------------------------------------------------"));
  Serial.println(F("You can type now:"));
}

void getColors() {
  byte bufPoz;

  for (byte x = 0; x < 6; x++) {
    serBuf[x] = Serial.read();
  }

  for (byte x = 0; x < 6; x++) {
    if (serBuf[x] > 0) {
      bufPoz++;
    }
  }

  for (byte x = 0; x < bufPoz; x++) {
    if (char(serBuf[x]) == '1') {
      wireColor[x] = c_RED;
    }
    if (char(serBuf[x]) == '2') {
      wireColor[x] = c_BLUE;
    }
    if (char(serBuf[x]) == '3') {
      wireColor[x] = c_YELLOW;
    }
    if (char(serBuf[x]) == '4') {
      wireColor[x] = c_WHITE;
    }
    if (char(serBuf[x]) == '5') {
      wireColor[x] = c_BLACK;
    }
  }

  Serial.println(F("wire colors in memory:"));
  for (byte x = 0; x < 6; x++) {
    if (wireColor[x] > 0) {
      Serial.print(wireColor[x], DEC);
    }
  }
  Serial.println(F(""));
  Serial.println(F("in position:"));
  for (byte x = 0; x < 6; x++) {
    if (wirePosition[x] > 0) {
      Serial.print(wirePosition[x]);
    }
  }
  Serial.println(F(""));
  Serial.println(F("-------------------------------------------------------"));
}

void getCutMask() {
  if (wireCount == 3) {
    if (isThere(c_RED) == 0) {
      Serial.println("No red");
      cutMask = wirePosition[1];
    }
    else if (wireColor[2] == c_WHITE) {
      Serial.println("last wire");
      cutMask = wirePosition[2];
    }
    else if (isThere(c_BLUE) >= 2) {
      Serial.println("last Blue");
      cutMask = wirePosition[lastWire(c_BLUE)];
    }
    else {
      Serial.println("no rule, last wire");
      cutMask = wirePosition[2];
    }
  }
  Serial.print(F("Cut the wire in position "));
  Serial.println(cutMask);
}

byte lastWire(byte color) {
  byte lastPlace = 0;
  for (byte x = 0; x < 6; x++) {
    if (wireColor[x] == color) {
      lastPlace = x;
    }
  }
  return lastPlace;
}

byte isThere(byte color) {
  byte colorCount = 0;
  for (byte x = 0; x < 6; x++) {
    if (wireColor[x] == color) {
      colorCount++;
    }
  }
  return colorCount;
}


void getWires() {
  for (byte x = 0; x < 6; x++) {
    In[x] = digitalRead(pins[x]);
  }
}
