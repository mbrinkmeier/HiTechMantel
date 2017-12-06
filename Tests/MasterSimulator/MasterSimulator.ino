#include <Wire.h>

char[256] request;
bool read;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(500);
  
  Serial.println("Initilized as master");
}

void loop() {
  read = false;
  while ( Serial.available() ) {
    read = true;
    byte b = Serial.readBytesUntil('\n',request,256);
  }

  if ( read ) {
    
  }
}
