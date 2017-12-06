#include <Wire.h>

#define ID 5

void setup() {
   Serial.begin(9600);
   
   Wire.begin(ID);
   Wire.onReceive(handleMsg);

   delay(1000);

   Serial.print("Listening as ID ");
   Serial.println(ID);
   Serial.println("Echoing I2C");

   delay(1000);
}

void loop() {
  delay(100);
}

void handleMsg(int numBytes) {
  while ( Wire.available() ) {
    byte b = Wire.read(); 
    Serial.println(b);
  }
}

