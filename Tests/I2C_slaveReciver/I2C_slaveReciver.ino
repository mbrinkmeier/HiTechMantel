#include <Wire.h>

#define ID 2

int led = 7;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delay(5000);
  digitalWrite(led, LOW);
  Wire.begin(ID);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  delay(1000);
  Serial.println("Receiving ... ");
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {

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
}
