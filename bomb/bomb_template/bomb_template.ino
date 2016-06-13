#include <Wire.h>

#define moduleID 11                                           //  ID of the module
#define moduleAddr 1                                          //  address of the module on I2C bus
#define ERR 4                                                 //  Error line
#define GREEN_LED 5                                           //  Green led on the module

#define NrInPins 3                                            //  Number of INPUTS
const byte pinsIn[NrInPins] = {7, 8, 9};                      //  pins INPUT
#define NrOutPins 2                                           //  Number of OUTPUTS
const byte pinsOut[NrOutPins] = {ERR, GREEN_LED};             //  pins OUTPUT

void analyzeBuf();                                            //  looks at first byte and calls fuctions
void getSerial();                                             //  get Serial number from buffer
byte ICbuf[10];                                               //  buffer for I2C bus
char moduleStat = 'E';                                        //  status of module (Enabled/Disabled)
boolean flagActive = false;                                   //  master activated this module
char serialNr[6];                                             //  seral number
byte optionBuf[9];                                            //  buf that hold option/setings for module
byte sendBuf[3];                                              //  Buffer that holds data for I2C (slave info)


void setup() {
  Serial.begin(9600);
  Wire.begin(moduleAddr);
  Wire.onReceive(rec);                                //  recive data from I2C bus
  Wire.onRequest(req);                                //  request data from I2C bus (device)
}

void loop() {
  if (flagActive) {
    //Serial.println("active!");
    //delay(1000);
  }
}

void rec(int in) {
  Serial.print("recived:");
  for (byte x = 0; x < in; x++) {
    ICbuf[x] = Wire.read();
    Serial.print(char(ICbuf[x]));
    Serial.print(F(","));
  }
  Serial.println();
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
  Serial.println(F("getOptions"));

}

