#include <Wire.h>

byte x = 0;

int i = 0;

byte message;

void setup() {
  Wire.begin();        // beginnt Übertagung
  Serial.begin(9600);  
}

void loop() {

  for(int j = 2; j <= 8; j++) {
    send(j, i);
    delay(1000);
  }

  i = (i + 1) % 2;
  
  delay(1000);
}

/*
 * Ruft eine Nachricht bei einem SlaveSender ab
 * @param slave Slave von dem die Nachricht empfangen wird
 * @param bytes Anzahl der erwarteten Bytes
 */
void recive(byte slave, byte bytes) {
  
  Wire.requestFrom(slave, bytes); 

  message = Wire.read();

}

/*
 * Sendet eine Nachricht an einen SlaveReciver
 * @param slave Slave, der die Nachricht empfangen soll
 * @param message zu sendende Message
 */
void send(int slave, byte message) {
  Wire.beginTransmission(slave); 
  Wire.write(message);              
  Wire.endTransmission();    
}

