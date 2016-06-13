#include <Wire.h>
#include "array.h"

const byte pins[2] = {2, 3};  // pins
char serialBuf[10];           // Buffer for serial input
byte ICbuf[10];               // guffer for I2C
char serialNr[6];             // serial number
byte modulesAddr[12];         // addres od each module
byte modulesType[12];         // type of module
byte modulesCounter = 0;      // Number of modules detected;
int timer = 0;                // bomb timer

void setup() {
  Serial.begin(9600);
  //while (!Serial);          //  leonardo...
  Wire.begin();
  setPins();
  setBomb();

  Serial.println(F("Modules are procesing data..."));
  delay(2500);
  Serial.println(F("Activate Bomb (send anything to activate)"));
  getResponse();
  activateBomb();
}

void loop() {
  checkModules();
  Time();
  delay(1000);
}

void Time() {
  Serial.print(F("Time left: "));
  Serial.print(timer);
  Serial.println(F("s"));
  if (timer == 0) {
    Detonate();
  }
  timer--;
}

void Detonate() {
  Serial.println(F("BuUuuuMMMmmm!"));
  while (true) {
    delay(100);
  }
}

void checkModules() {
  byte modulesDeactiv = 0;      // how many modules did homan deactivated
  for (byte x = 0; x < modulesCounter; x++) {
    Wire.requestFrom(modulesAddr[x], 3);
    getBuff(modulesAddr[x]);
    if (ICbuf[1] == 'D') {
      Serial.print(F("Module deactivated at adr "));
      Serial.println(modulesAddr[x], HEX);
      modulesDeactiv++;
    }
    else if (ICbuf[1] == 'E') {
      Serial.print(F("Active module at addr "));
      Serial.println(modulesAddr[x], HEX);
    }
  }
  if (modulesDeactiv == modulesCounter) {
    win();
  }
}


void win() {
  Serial.println("Bomb deactivated, you win");
  while (true);
}

void activateBomb() {
  byte checkErr = 0;
  for (byte x = 0; x < modulesCounter; x++) {      //  every device on list
    Wire.beginTransmission(modulesAddr[x]);
    for (byte y = 0; y < 6; y++) {
      Wire.write('A');                    //  send activation code
    }
    checkErr = Wire.endTransmission();
    if (checkErr != 0) {
      Serial.print(F("Error on addr "));
      Serial.println(modulesAddr[x]);
      checkErr = 0;
    }
    else if (checkErr == 0) {
      Serial.print(F("Modul on addr "));
      Serial.print(modulesAddr[x]);
      Serial.println(F(" Activated"));
    }
  }

}

void setPins() {
  //set up inputs/outpust
}

void setBomb() {
  info(); //print info in serial
  checkForWires();
  setTime();
}

void checkForWires() {
  for (byte x = 0; x < 12; x++ ) {
    if (modulesType[x] == 3) {
      setUpWires(modulesAddr[x]);
    }
  }
}

void setTime() {
  Serial.println(F("Set the timer on bomb"));
  Serial.println(F("please type in total seconds:"));
  while (Serial.available() == 0) {          //  wait for response
    delay(500);
  }
  timer = Serial.parseInt();
  Serial.print(F("Timer set to "));
  Serial.print(timer);
  Serial.println(F("s"));
}

void setUpWires(byte addr) {
  Serial.print(F("Found a wires module at adrress: "));
  Serial.println(addr, HEX);
  Serial.println(F("Please type in thier color with int value from table below,"));
  Serial.println(F("in format '121554'."));
  Serial.println(F("1 = RED"));
  Serial.println(F("2 = BLUE"));
  Serial.println(F("3 = YELLOW"));
  Serial.println(F("4 = WHITE"));
  Serial.println(F("5 = BLACK"));
  getResponse();
  Wire.beginTransmission(addr);
  Wire.write('O');
  for (byte x = 0; x < 6; x++)   {
    Wire.write(serialBuf[x]);
  }
  byte checkErr = Wire.endTransmission();
  if (checkErr != 0) {
    Serial.print(F("Error on addr "));
    Serial.println(addr);
    checkErr = 0;
  }
  else if (checkErr == 0) {
    Serial.print(F("Wire colors send to "));
    Serial.println(addr);
  }
  Serial.println(F("-----------------------------------------------------"));
}

void getResponse() {
  while (Serial.available() == 0) {          //  wait for response
    delay(250);
  }
  byte con = 0;
  while (Serial.available()) {
    serialBuf[con] = Serial.read();     // get the response to buffer for later use
    con++;
  }
  Serial.flush();
}

void info() {
  Serial.println(F("Keep talking and nobody explodes - Timer module"));
  Serial.println(F("This module is used to set up bomb, please make sure,"));
  Serial.println(F("that the bomb is fully set up before typing any info."));
  Serial.println(F("-----------------------------------------------------"));
  Serial.println(F("Now scaning the Bomb bus for modules..."));
  scanBus();
  Serial.println(F("-----------------------------------------------------"));
  Serial.print(F("Bomb Serial number (send anything to generate): "));
  bombSerial();
  Serial.println(F("Sending Serial number to modules..."));
  bombSerialSend();
}

void bombSerialSend() {
  byte checkErr;
  for (byte x = 0; x < modulesCounter; x++) {      //  every device on list
    Wire.beginTransmission(modulesAddr[x]);
    Wire.write('S');
    for (byte y = 0; y < 6; y++) {
      Wire.write(serialNr[y]);                    //  send serial number
    }
    checkErr = Wire.endTransmission();
    if (checkErr != 0) {
      Serial.print(F("Error on addr "));
      Serial.println(modulesAddr[x]);
      checkErr = 0;
    }
    else if (checkErr == 0) {
      Serial.print(F("Serial number send to "));
      Serial.println(modulesAddr[x]);
    }
  }
  Serial.println(F("Sending serial number done"));
  Serial.println(F("-----------------------------------------------------"));
}

void bombSerial() {
  getResponse();
  randomSeed(millis());
  for (byte x = 0; x < 5; x++) {
    serialNr[x] = lett[random(0, 61)];
  }
  serialNr[5] = lett[random(52, 61)]; //last position of serial number is always number

  for (byte x = 0; x < 6; x++) {
    Serial.print(char(serialNr[x]));
  }
  Serial.println();
}

void scanBus() {
  byte error = 0;
  byte type = 0;
  for (byte addr = 1; addr <= 127; addr++) {
    Wire.beginTransmission(addr);
    error = Wire.endTransmission();
    //Serial.println(addr, HEX);
    if (error == 0) {
      Serial.print(F("Found "));
      Wire.requestFrom(addr, 2);
      getBuff(addr);
      Serial.print(modules[ICbuf[0]]);
      Serial.print(F(" at addres "));
      Serial.println(addr, HEX);
      modulesAddr[modulesCounter] = addr;
      modulesType[modulesCounter] = ICbuf[0];
      modulesCounter++;
    }
  }
  if (modulesCounter == 0) {
    Serial.println(F("No modules found !"));
  }
  else {
    Serial.print(F("Scan done, Found "));
    Serial.print(modulesCounter, DEC);
    Serial.println(F(" modules"));
  }
}

void getBuff(byte addr) {
  /*
    while (!Wire.available()) {
    delay(1);
    }
  */
  byte cont = 0;
  while (Wire.available()) {
    ICbuf[cont] = Wire.read();
    cont++;
  }
  Serial.print("recived: ");
  for (byte x = 0; x < 10; x++) {
    Serial.print(ICbuf[x]);
    Serial.print(",");
  }
  Serial.print(F(" from "));
  Serial.println(addr, HEX);
}

