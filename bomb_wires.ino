#include <Wire.h>
#include "def.h"

const byte pins[6] PROGMEM = {2, 3, 4, 5, 6, 7};
byte In[6];
byte mask[6];


void setup() {
  for (byte x; x < 6; x++) {
    pinMode(pgm_read_byte_near(pins[x]), INPUT_PULLUP);
  }
}

void loop() {
  getWires();
  Check();
}

void Check() {
  byte ErrFlag;                   //bonzák
  for (byte x; x < 6; x++) {
    if (mask[x] != In[x]) {       //porovnat kabely vuči masce
      ErrFlag++;                //error in wire
      mask[x] = In[x]];           //remember that error in next round
    }
  }
  if (ErrFlag > 0) {
    while (ErrFlag > 0) {
      handleERR();
      ErrFlag--;
    }
  }
}

void hanndleERR() {
  digitalWrite(ERR, LOW);
  delay(10);
  digitalWrite(ERR, HIGH);
}

void getWires() {
  for (byte x; x < 6; x++) {
    In[x] = digitalRead(pgm_read_byte_near(pins[x]));
  }
}

