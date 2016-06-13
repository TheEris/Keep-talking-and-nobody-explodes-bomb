#include <Wire.h>
#include "def.h"

const byte pins[6] = {4, 5, 6, 7, 8, 9};              //  input pins for wires, pullups
byte In[6];                                           //  holds the pin status
byte serBuf[10];                                      //  temp buffer for serial
byte ICbuf[10];                                       //  buffer for I2C bus

byte wireCount = 0;                                   //  number of wires
byte wirePosition[6];                                 //  where are they
byte wireColor[6];                                    //  wire colors
byte cutMask = 0;                                     //  position where cut the wire
byte mask[6];                                         //  mask for cheking the wires
char serialNr[6];                                     //  Serial number
byte generated = 0;                                   //  did you generated the mask ?

boolean flagActive = false;                           //  is modul active ?
boolean flagSerial = false;                           //  serial number from master has arrived
boolean flagColors = false;                           //  colors from master has arrived

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Wire.begin(2);
  Wire.onReceive(rec);                                //  recive data from I2C bus
  Wire.onRequest(req);                                //  request data from I2C bus (device)

  for (byte x = 0; x < 6; x++) {
    pinMode(pins[x], INPUT_PULLUP);
  }

  pinMode(ERR, OUTPUT);
  pinMode(DET, OUTPUT);
  pinMode(win_LED, OUTPUT);

  scanInputs();
  setColors();
  getCutMask();
}

void loop() {
  if (flagActive) {
    getWires();
    Check();
  }
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

void analyzeData() {
  if (ICbuf[0] == '1') {
    Serial.println("flagSerial true");
    flagSerial = true;
  }
  else if (ICbuf[0] == '2') {
    Serial.println("flagColors true");
    flagColors = true;
  }
  else if (ICbuf[0] == 'A') {
    Serial.println("flagActive true");
    flagActive = true;
  }
  checkFlags();
}

void checkFlags() {
  if (flagSerial == true) {
    for (byte x = 0; x < 6; x++) {
      serialNr[x] = ICbuf[x + 1];
    }
    Serial.print(F("Serial number: "));

    for (byte x = 0; x < 10; x++) {
      Serial.print(serialNr[x]);
    }
    Serial.println(F(""));
    flagSerial = false;
  }
}

void setColors() {
  Serial.println("cekm na na colors");
  while (flagColors == false) {
    delay(100);                            //wait for data from master;
  }
  flagColors = false;
  Serial.println("got colors");
  for (byte x = 0; x < 6; x++) {
    wireColor[x] = ICbuf[x + 1] - 48;
    Serial.print(wireColor[x], DEC);
    Serial.println();
  }
}

void getCutMask() {
  byte lastDigit = serialNr[5] - 48;
  Serial.print (F("last Serial number: "));
  Serial.println (lastDigit);

  if (wireCount == 3) {
    Serial.println(F("3 wires"));
    if (isThere(c_RED) == 0) {
      Serial.println(F("No red"));
      cutMask = wirePosition[1];
    }
    else if (wireColor[2] == c_WHITE) {
      Serial.println(F("last wire"));
      cutMask = wirePosition[2];
    }
    else if (isThere(c_BLUE) >= 2) {
      Serial.println(F("last Blue"));
      cutMask = wirePosition[lastWire(c_BLUE)];
    }
    else {
      Serial.println(F("no rule, last wire"));
      cutMask = wirePosition[2];
    }
  }
  // ------------------------------------------
  else if (wireCount == 4) {
    Serial.println(F("4 wires"));
    if (isThere(c_RED) > 1 && !(lastDigit % 2 == 0) ) {
      Serial.println(F("1+red, odd last serial number"));
      cutMask = wirePosition[lastWire(c_RED)];
    }
    else if (wireColor[3] == c_YELLOW && isThere(c_RED) == 0) {
      Serial.println(F("last wire yellow, no red"));
      cutMask = wirePosition[0];
    }
    else if (isThere(c_BLUE) == 1) {
      Serial.println(F("one blue"));
      cutMask = wirePosition[0];
    }
    else if (isThere(c_YELLOW) > 1) {
      Serial.println(F("1+Yellow"));
      cutMask = wirePosition[3];
    }
    else {
      Serial.println(F("no rule"));
      cutMask = wirePosition[1];
    }
  }
  // ------------------------------------------
  else if (wireCount == 5) {
    Serial.println(F("5 wires"));
    if (wireColor[4] == c_BLACK && !(lastDigit % 2 == 0)) {
      Serial.println(F("last black, odd number"));
      cutMask = wirePosition[3];
    }
    else if (isThere(c_RED) == 1 && isThere(c_YELLOW) > 1) {
      Serial.println(F("1 red, +1yellow"));
      cutMask = wirePosition[0];
    }
    else if (isThere(c_BLACK) == 0) {
      Serial.println(F("no black"));
      cutMask = wirePosition[1];
    }
    else {
      Serial.println(F("no rule"));
      cutMask = wirePosition[0];
    }
  }
  // ------------------------------------------

  else if (wireCount == 6) {
    Serial.println(F("6 wires"));
    if (isThere(c_YELLOW) == 0 && (lastDigit % 2 == 0)) {
      Serial.println(F("no yellow, odd number"));
      cutMask = wirePosition[2];
    }
    else if (isThere(c_YELLOW) == 1 && isThere(c_WHITE) > 1) {
      cutMask = wirePosition[3];
    }
    else if (isThere(c_RED) == 0) {
      cutMask = wirePosition[5];
    }
    else {
      cutMask = wirePosition[3];
    }
  }

  // ------------------------------------------

  else {
    Serial.println(F("Error"));
    while (true) {};
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

void Check() {
  byte pinChange = 0;   //did pins changed ?

  for (byte x = 0; x < 6; x++) {
    if (In[x] != mask[x] ) {
      mask[x] = In[x];
      pinChange++;
    }
  }

  if (pinChange > 0 && generated == 1) {
    pinChange = 0;
    if (In[cutMask - 1] == 1) {
      win();
    }
    else {
      Error();
    }
  }
  generated = 1;
}

void win() {
  //module deactivated
  digitalWrite(win_LED, HIGH);
  Serial.println(F("You win!"));
                                                        //  need to tell to the master
  while (true) {}; //do nothing until restart
}

void Error() {
  Serial.println(F("nope!"));
  digitalWrite(ERR, LOW);
  delay(10);
  digitalWrite(ERR, HIGH);
}

void rec(int in) {
  Serial.print("recived:");
  for (byte x = 0; x < in; x++) {
    ICbuf[x] = Wire.read();
    Serial.print(char(ICbuf[x]));
    Serial.print(",");
  }
  Serial.println();
  analyzeData();
}

void req() {
  Wire.write(moduleID);
}

