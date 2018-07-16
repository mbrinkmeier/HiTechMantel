/**
 * Uno SDA: A4, SCL: A5
 */
#include <Wire.h>

int ID = 1;

int led = 7;
byte zaehler = 0;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  Wire.begin(ID);                // join i2c bus with address #8
  Serial.begin(9600);           // start serial for output
  delay(500);
  Serial.println("Receiving ... ");
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
 
  if (Wire.available()) {
    // int x = Wire.read();    // receive byte as an integer
    // Serial.println(x);
    // if(x%2 == 0) {
    //   digitalWrite(led, LOW);
    // } else if(x%2 == 1) {
    //   digitalWrite(led, HIGH);
    // }
    while ( Wire.available() ) {
      Serial.print((int) Wire.read(),DEC);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void requestEvent() {
  Serial.println("Request");
  Serial.print("Answer ");
  Serial.println(zaehler);
  Wire.write(zaehler++);
/*
  if (Wire.available()) {
    int x = Wire.read();    // receive byte as an integer
    Serial.println(x);
    if(x%2 == 0) {
      digitalWrite(led, LOW);
    } else if(x%2 == 1) {
      digitalWrite(led, HIGH);
    }
    while ( Wire.available() ) {
      Serial.print((char) Wire.read());
    }
    Serial.println();
  }
  */
}
