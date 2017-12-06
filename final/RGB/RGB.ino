/**
 * This sketch is the programm for the thre RGB controlling Floras.
 * 
 * It receives the red, green and blue component to be displayed.
 */
#include <Wire.h>
#include <HiTechMantel.h>


// The pins for the RGB-LED
#define RED 10
#define GREEN 9
#define BLUE 6

// The id of the flora

// int id = ID_BACK;
int id = ID_BELT;
// int id = ID_ARM;

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

/**
 * Set the Flora up.
 */
void setup() {
  // Start the serial connection for debugging.
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

  Serial.print("Listening on I2C with ID ");
  Serial.println(id);
}


/**
 * The main loop does nothing.
 */
void loop() {
  delay(100);
}


/* 
 * Handle a received message
 */
void receiveEvent(int numBytes) {
  byte cmd;   // The command id
  byte dlen;  // The data length
  byte data[255];   // The data bytes

  cmd = mantel.readFromWire();
  dlen = mantel.readFromWire();

  Serial.print("Received cmd: ");
  Serial.print(cmd);
  Serial.print(" dlen: ");
  Serial.println(dlen);

  mantel.readData(dlen,data);
  
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
      analogWrite(RED,data[0]);
      analogWrite(GREEN,data[1]);
      analogWrite(BLUE,data[2]);
      Serial.println(data[0]);
      Serial.println(data[1]);
      Serial.println(data[2]);
      pixel.setPixelColor(0,data[0],data[1],data[2]);
      pixel.show();
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

