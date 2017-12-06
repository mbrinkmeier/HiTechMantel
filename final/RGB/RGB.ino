/**
 * This sketch is the programm for the thre RGB controlling Floras.
 * 
 * It receives the red, green and blue component to be displayed.
 */
#include <Wire.h>
#include <HiTechMantel.h>

#define RED 10
#define GREEN 9
#define BLUE 6

// int id = ID_BACK;
// int id = ID_BELT;
int id = ID_ARM;

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
  Serial.begin(9600);

  // Set the pin mode for the RGB Pins
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Set all colors to zero
  analogWrite(RED,0);
  analogWrite(GREEN,0);
  analogWrite(BLUE,0);

  // Run the selftest
  selftest();

  // Start the i2c conection
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  Serial.print("Listening with ID ");
  Serial.println(id);
}


void loop() {
  delay(100);
}

byte odd = 0;

/* 
 * Handle the received bytes 
 */
void receiveEvent(int numBytes) {
  byte cmd;
  byte red;
  byte green;
  byte blue;

  cmd = Wire.read();
  Serial.println(cmd);
  
  switch (cmd) {
    case CMD_RGB_RESET:
      analogWrite(RED,0);
      analogWrite(GREEN,0);
      analogWrite(BLUE,0);
      break;
    case CMD_SELFTEST:
      selftest();
      break;
    case CMD_RGB_SET:
      // Set the color
      red = Wire.available() ? Wire.read() : 0;
      green = Wire.available() ? Wire.read() : 0;
      blue = Wire.available() ? Wire.read() : 0;
      analogWrite(RED,red);
      analogWrite(GREEN,green);
      analogWrite(BLUE,blue);
      Serial.println(red);
      Serial.println(green);
      Serial.println(blue);
      break;
   case CMD_PING:
      Wire.onRequest(waitForPing);
      break;
  }
}


/**
 * Wait for the ping request.
 */
void waitForPing() {
  Wire.write("ping");
  Wire.onRequest(NULL);
}

