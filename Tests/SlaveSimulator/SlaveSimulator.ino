#include <Wire.h>

#define ID 5

char buffer[100];

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
  if (Serial.available()) {
    long id = Serial.parseInt();
    if ( id != 0 ) {
      Serial.println(">> Switching to ID " + String(id));

      Wire.begin((int) id);
      Wire.onReceive(handleMsg);

      delay(1000);

      Serial.print("Listening as ID ");
      Serial.println(id);
      Serial.println("Echoing I2C");      
    }
  }
}


void handleMsg(int numBytes) {
  while ( Wire.available() ) {
    byte b = Wire.read(); 
    Serial.println(b);
  }
}

