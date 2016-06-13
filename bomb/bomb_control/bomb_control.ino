#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();

  scanBus();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void scanBus() {
  Serial.print(F("Scaning..."));
  for (byte x = 1; x < 127; x++) {
    Wire.beginTransmission(x);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(F("Found module on addr: "));
      Serial.println(x, HEX);
    }
  }
}

