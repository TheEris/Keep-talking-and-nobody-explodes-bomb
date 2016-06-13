#include <Wire.h>
#include <morse.h>
#include "morseArray.h"

#define moduleID 11                                           //  ID of the module
#define morsePin 13                                           //  pin with yellow diode
#define ERR 4                                                 //  Error line
#define GREEN_LED 5                                           //  Green led on the module

#define NrInPins 3                                            //  Number of INPUTS
const byte pins[NrInPins] = {7, 8, 9};                        //  pins INPUT
#define NrOutPins 3                                           //  Number of OUTPUTS
const byte pinsOUT[NrOutPins] = {ERR, GREEN_LED, morsePin};   //  pins OUTPUT
byte ICbuf[10];                                               //  buffer for I2C bus
char moduleStat = 'A';                                        //  status of module (active/deactivated)
boolean flagActive = false;                                   //  master activated this module
boolean flagSerial = false;                                   //  serial number is in buffer
String msg;                                                   //  msg in morse code
char serialNr[6];                                             //  seral number
byte counterF = 0;                                            //  hold actual frq in display
byte Anw = 0;                                                 //  right anwser


LEDMorseSender cqSender(pinsOUT[2], 6.0);

void setup() {
  Serial.begin(9600);
  while (!Serial);                                    //  coz leonardo is bitch
  Wire.begin(1);                                      //  slave on addr 6
  Wire.onReceive(rec);                                //  recive data from I2C bus
  Wire.onRequest(req);                                //  request data from I2C bus (device)
  setUpPins();                                        //  set up pins
  cqSender.setup();

  Serial.println("awaithing for activating flag");
  while (!flagActive) {
    delay(100);
  }

  getMsg();
  cqSender.setMessage(msg);
  Serial.println("msg set");
  displayFreq(counterF);

  digitalWrite(pinsOUT[1], LOW);
}

void loop() {
  if (flagActive == true) {
    if (!cqSender.continueSending()) {
      cqSender.startSending();
    }
    checkButtons();
  }
  if (moduleStat == 'D') {
    while (true) {
      delay(100);
    }
  }
}

void checkButtons() {
  if (!digitalRead(pins[0])) {
    counterF++;
    if (counterF > 16) {                              //  make sure its in range
      counterF = 16;
    }
    displayFreq(counterF);                            //  display it
  }
  else if (!digitalRead(pins[1])) {
    counterF--;
    if (counterF == 0) {                              //  make sure its in range
      counterF = 1;
    }
    displayFreq(counterF);                            //  display it
  }
  else if (!digitalRead(pins[2])) {
    if ((counterF - 1) == Anw) {
      Win();
    }
    else {
      Error();
    }
  }
  delay(100);
}

void Error() {
  digitalWrite(ERR, LOW);
  delay(100);
  digitalWrite(ERR, HIGH);
}

void Win() {
  moduleStat = 'D';                               //  tell master human did it
  digitalWrite(GREEN_LED, HIGH);
  Serial.println("winner");
  flagActive = false;
}

void displayFreq(byte freq) {
  Serial.print("3.");
  Serial.print(morseFreq[freq - 1]);
  Serial.println("Mhz");
}

void getMsg() {
  randomSeed(millis());
  Anw = random(0, 16);
  msg = morseWords[Anw];
  counterF = random(1, 17);
  Serial.print("msg is: ");
  Serial.println(msg);
}

void setUpPins() {
  for (byte x = 0; x < NrInPins; x++) {
    pinMode(pins[x], INPUT_PULLUP);
  }
  for (byte x = 0; x < NrOutPins; x++) {
    pinMode(pinsOUT[x], OUTPUT);
    digitalWrite(pinsOUT[x], HIGH);
  }
}

void req() {
  Wire.write(moduleID);
  Wire.write(moduleStat);
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

void analyzeData() {
  if (ICbuf[0] == 'S') {
    Serial.println("flagSerial");
    flagSerial = true;
  }
  else if (ICbuf[0] == 'A') {
    Serial.println("flagActive");
    flagActive = true;
  }
  checkFlags();
}

void checkFlags() {
  if (flagSerial) {
    for (byte x = 0; x < 6; x++) {
      serialNr[x] = ICbuf[x + 1];
    }
    flagSerial = false;
  }
}

