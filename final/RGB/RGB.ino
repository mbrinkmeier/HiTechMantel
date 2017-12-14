/** 
 *  This sketch is the programm for the three RGB LEDs controlling Floras in
 *  the HiTechCoat Project.
 * 
 *  The RGB Led (WS2812B) is connected to LED_PIN (10).
 *  
 *  
 *  Copyright (c) 2017 Michael Brinkmeier (michael.brinkmeier@uni-osnabrueck.de)
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
#include <Wire.h>
#include <HiTechMantel.h>

#define debugSerial Serial
#define SHOW_DELAY 50
#define PIXEL_PIN 10

// The id of the flora
// int id = ID_BACK;
int id = ID_BELT;
// int id = ID_ARM;

bool debugging = true;

int red = 0;
int green = 0;
int blue = 0;

bool colorChanged = false;

HiTechMantel mantel = HiTechMantel();

// Floras own pixel
Adafruit_NeoPixel pixel = mantel.pixel;                                  

// the external pixel
Adafruit_NeoPixel pixel2 = Adafruit_NeoPixel(1,PIXEL_PIN,NEO_GRBW + NEO_KHZ800);


/**
 * Set the Flora up.
 */
void setup() {
  // Start the serial connection for debugging.
  debugSerial.begin(9600);

  // Initialize the two pixel
  pixel.begin();
  pixel2.begin();

  // Run the selftest
  selftest();

  // Start the i2c conection
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  DBG_PRINT(F("Listening on I2C with ID "));
  DBG_PRINTLN(id);
}


/**
 * The main loop does nothing.
 */
void loop() {
  // If the color changed, update the pixels
  if (colorChanged) {
    setColor();
    colorChanged = false;
  }
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

  mantel.readData(dlen,data);
  mantel.emptyWire();

  DBG_PRINT(F("Received cmd: "));
  DBG_PRINT(cmd);
  DBG_PRINT(F(" dlen: "));
  DBG_PRINT(dlen);
  if (debugging) mantel.debugData(data,dlen);
  DBG_PRINTLN("");
  
  switch (cmd) {
    case CMD_SELFTEST:
      // Run selftest
      selftest();
      // then reset
    case CMD_RGB_RESET:
      // set color to black
      red = 0;
      green = 0;
      blue = 0;
      colorChanged = true;
      break;
    case CMD_RGB_SET:
      // Set the color
      red = data[0];
      green = data[1];
      blue = data[2];
      colorChanged = true;
      break;
   case CMD_RGB_RED:
      red = data[0];
      colorChanged = true;
      break;
   case CMD_RGB_GREEN:
      green = data[0];
      colorChanged = true;
      break;
   case CMD_RGB_BLUE:
      blue = data[0];
      colorChanged = true;
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

  pixel2.setPixelColor(0,255,0,0);
  pixel2.show();
  pixel.setPixelColor(0,255,0,0);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,255,0);
  pixel2.show();
  pixel.setPixelColor(0,0,255,0);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,0,255);
  pixel2.show();
  pixel.setPixelColor(0,0,0,255);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,255,255,0);
  pixel2.show();
  pixel.setPixelColor(0,255,255,0);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,255,0,255);
  pixel2.show();
  pixel.setPixelColor(0,255,0,255);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,255,255);
  pixel2.show();
  pixel.setPixelColor(0,0,255,255);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,0,0);
  pixel2.show();
  pixel.setPixelColor(0,0,0,0);
  pixel.show();
  delay(500);
}



/**
 * Set the color of both pixels
 */
void setColor() {
  pixel2.setPixelColor(0,red,green,blue);
  pixel2.show();
  delay(SHOW_DELAY);
  
  pixel.setPixelColor(0,red,green,blue);
  pixel.show();
  delay(SHOW_DELAY);
}

