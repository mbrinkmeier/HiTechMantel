/**
 * 
 */
#include <Wire.h>
#include <HiTechMantel.h>

#define RED 10
#define GREEN 9
#define BLUE 6


int id = BACK_ID;

/** 
 * Selftest
 * 
 * Cycle through all colors.
 */
void selftest() {

  analogWrite(RED,255);
  analogWrite(GREEN,0);
  analogWrite(BLUE,0);

  delay(500);
  
  analogWrite(RED,255);
  analogWrite(GREEN,255);
  analogWrite(BLUE,0);

  delay(500);
  
  analogWrite(RED,0);
  analogWrite(GREEN,255);
  analogWrite(BLUE,0);

  delay(500);
  
  analogWrite(RED,0);
  analogWrite(GREEN,255);
  analogWrite(BLUE,255);

  delay(500);
  
  analogWrite(RED,0);
  analogWrite(GREEN,0);
  analogWrite(BLUE,255);

  delay(500);
  
  analogWrite(RED,255);
  analogWrite(GREEN,0);
  analogWrite(BLUE,255);

  delay(500);
  
  analogWrite(RED,0);
  analogWrite(GREEN,0);
  analogWrite(BLUE,0);
}



void setup() {
  // Set the pin mode for the RGB Pins
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Set all colors to zero
  analogWrite(RED,0);
  analogWrite(GREEN,0);
  analogWrite(BLUE,0);
  
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
}


void loop() {
  delay(100);
}


/* 
 * Handle the received bytes 
 */
void receiveEvent(int numBytes) {
  char reg;
  char red;
  char green;
  char blue;

  reg = Wire.read();

  switch (reg) {
    case RESET_REG:
      analogWrite(RED,0);
      analogWrite(GREEN,0);
      analogWrite(BLUE,0);
      break;
    case SELFTEST_REG:
      selftest();
      break;
    case RGB_SET_REG:
      // Set the color
      red = Wire.available() ? Wire.read() : 0;
      green = Wire.available() ? Wire.read() : 0;
      blue = Wire.available() ? Wire.read() : 0;
      analogWrite(RED,red);
      analogWrite(GREEN,green);
      analogWrite(BLUE,blue);
      break;
  }

}

