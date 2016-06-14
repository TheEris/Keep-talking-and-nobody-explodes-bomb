#include <Wire.h>

#define moduleID 11                                           //  ID of the module
#define moduleAddr 1                                          //  address of the module on I2C bus
#define ERR 4                                                 //  Error line
#define GREEN_LED 5                                           //  Green led on the module

byte ICbuf[10];                                               //  buffer for I2C bus
char moduleStat = 'E';                                        //  status of module (Enabled/Disabled)
boolean flagActive = false;                                   //  master activated this module
char serialNr[6];                                             //  seral number
byte optionBuf[9];                                            //  buf that hold option/setings for module
byte sendBuf[3];                                              //  Buffer that holds data for I2C (slave info)
//---------------------------------------------------------------------------------------------------------------
//                                      module specific
//---------------------------------------------------------------------------------------------------------------
boolean errorFlag = false;                                    //  Error
boolean flagColors = false;                                   //  colors from master has arrived
byte wireCount = 0;                                           //  number of wires
byte wirePosition[6];                                         //  where are they
byte wireColor[6];                                            //  wire colors
byte cutMask = 0;                                             //  position where cut the wire
byte mask[6];                                                 //  mask for cheking the wires
boolean generated = false;                                    //  did you generated the mask ?
const byte pins[6] = {4, 5, 6, 7, 8, 9};                      //  input pins for wires, pullups
byte In[6];                                                   //  holds the pin status

#define ERR 13                                                //  Error line for timer
#define DET 12                                                //  Detonation line for timer (only for needy modules)
#define win_LED A0                                            //  green led on module

#define moduleID 3                                            //  ID of the module

#define c_RED 1
#define c_BLUE 2
#define c_YELLOW 3
#define c_WHITE 4
#define c_BLACK 5

void setup() {
  Serial.begin(9600);
  Wire.begin(moduleAddr);
  Wire.onReceive(rec);                                        //  recive data from I2C bus
  Wire.onRequest(req);                                        //  request data from I2C bus (device)

  setupPins();
  scanInputs();
  while (!flagColors) {
    delay(100);
  }
  getCutMask();
}

void loop() {
  if (flagActive) {
    getWires();
    Check();
  }
  delay(100);
}

void setupPins() {
  for (byte x = 0; x < 6; x++) {
    pinMode(pins[x], INPUT_PULLUP);
  }
}

void rec(int in) {
  for (byte x = 0; x < in; x++) {
    ICbuf[x] = Wire.read();
  }
  analyzeBuf();
}

void req() {
  sendBuf[0] = moduleID;
  sendBuf[1] = moduleStat;
  sendBuf[2] = 66;
  Wire.write(sendBuf, 3);
}

void analyzeBuf() {
  if (ICbuf[0] == 'A') {
    flagActive = true;
    Serial.println(F("Module Active !"));
  }
  else if (ICbuf[0] == 'S') {
    getSerial();
  }
  else if (ICbuf[0] == 'O') {
    getOptions();
  }
}

void getSerial() {
  Serial.println(F("getSerial"));
  for (byte x = 0; x < 6; x++) {
    serialNr[x] = ICbuf[x + 1];
    Serial.print(serialNr[x]);
  }
  Serial.println();
}

void getOptions() {
  Serial.println(F("getOptions/getColors"));
  for (byte x = 0; x < 6; x++) {
    wireColor[x] = ICbuf[x + 1] - 48;
    Serial.print(char(wireColor[x] + 48));
  }
  Serial.println();
  flagColors = true;
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

  if (pinChange > 0 && generated) {
    pinChange = 0;
    if (In[cutMask - 1] == 1) {
      win();
    }
    else {
      Error();
    }
  }
  generated = true;
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
    errorFlag = true;
    while (true) {
      delay(100);
    }
  }
}

void win() {
  Serial.println(F("Module deactivated !"));
  digitalWrite(win_LED, HIGH);
  moduleStat = 'D';
  flagActive = false;
}

void Error() {
  Serial.println(F("Wrong wire !"));
  //  Error
}

