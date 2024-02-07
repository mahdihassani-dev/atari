#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2);

int counter = 0;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {

  int wifiState = 0;

  while (mySerial.available()) {
    wifiState = mySerial.read();
    // Serial.println(wifiState);
  }

  if (wifiState == 200){
    turnOn();
    delay(3000);
  }else{
    blynk();
  }

  if (counter < 4) {
    mySerial.write(99);
    counter++;
  }
}

void blynk() {
  digitalWrite(13, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(200);              // wait for a second
  digitalWrite(13, LOW);   // turn the LED off by making the voltage LOW
  delay(200);
}

void turnOn() {
  digitalWrite(13, HIGH);
}