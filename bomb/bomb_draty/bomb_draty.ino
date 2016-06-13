#define ERR 13
#define DET 12
#define statusLed 10

byte wires[6];
char* serialNumber[10];

/*
   0 - null
   1 - red
   2 - blue
   3 - yellow
   4 - black
*/

void setup() {
  Serial.begin(9600);
  while (!Serial) ;

  for (byte x; x < 6; x++) {
    pinMode(x + 2, INPUT_PULLUP);
  }
  pinMode(statusLed, OUTPUT);
  pinMode(ERR, OUTPUT);
  pinMode(DET, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    setWires();
  }
}


void setWires() {
  



}

