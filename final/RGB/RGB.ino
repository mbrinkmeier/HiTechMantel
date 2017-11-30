#include <Wire.h>

// Id of the Slave
#define BACK_ID 20
#define ARM_ID 21
#define BELT_ID 22

#define RED 10
#define GREEN 9
#define BLUE 6

int id = BACK_ID;

void setup() {
  // Set the pin mode for the RGB Pins
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Set all colors to zero
  analoglWrite(RED,0);
  analoglWrite(GREEN,0);
  analoglWrite(BLUE,0);
  
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
}


void loop() {
  // put your main code here, to run repeatedly:
}


// Handle the received bytes
void receiveEvent(int numBytes) {
  char reg;
  char red;
  char green;
  char blue;

  reg = Wire.read();

  // Set the color
  if (reg == 0) {
     red = Wire.available() ? Wire.read() : 0;
     green = Wire.available() ? Wire.read() : 0;
     blue = Wire.available() ? Wire.read() : 0;
     analogWrite(RED,red);
     analogWrite(GREEN,green);
     analogWrite(BLUE,blue);
  }

  // Ignore other registers
  
}

