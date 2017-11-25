#include <Wire.h>

int i = 0;

void setup() {
  Wire.begin(1);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(i); // respond with message of 6 bytes

  i = (i + 1) % 2;
  
}
