/** 
 *  This sketch is the programm for I2C Master of the HiTechCoat Project.
 *  It communicates with the Nextion TouchScreen and relays commands to the other
 *  Floras and the LilyPad Mp3.
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
 #include <I2C.h>
#include <HiTechMantel.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_NeoPixel.h>

#define screenSerial Serial1
#define debugSerial Serial

#define PULSE_PIN A0
#define PULSE_INTERVAL 500
#define PULSE_AVG_COUNT 10

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

// Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1,8,NEO_GRBW + NEO_KHZ800);

byte readByteFromScreen() {
  byte b = screenSerial.available() ? screenSerial.read() : 0;
  delay(1);
  return b;
}

void readBytesFromScreen(int len, byte buf[]) {
  buf[0] = len;
  for (int i = 1; i <= len; i++) {
    buf[i] = readByteFromScreen();
  }
}

/*
void sendByte(byte id, byte data) {
  Wire.beginTransmission(id);
  Wire.write(data);
  Wire.endTransmission();
}

void sendArray(byte id, byte data[], int length) {
  Wire.beginTransmission(id);
  Wire.write(data,length);
  Wire.endTransmission();
  delay(100);
}
*/


/**
 * Setup as I2C master
 */
void setup() {
  debugSerial.begin(9600);
  screenSerial.begin(9600);
  
  // pixel.begin();
  // Do a selftest
  pixel.setPixelColor(0,255,0,0);
  pixel.show();
  delay(500);
  pixel.setPixelColor(0,0,255,0);
  pixel.show();
  delay(500);
  pixel.setPixelColor(0,0,0,255);
  pixel.show();
  delay(500);
  pixel.setPixelColor(0,0,0,0);
  pixel.show();
  delay(500);
  
  // Initialize I2C as master
  I2c.begin();
  I2c.timeOut(1000);

  // Initialize serial connections
  debugSerial.println(F("Init done"));
  debugSerial.println(F("Listen as master"));
}


/**
 * Recevie messages fromthe screen
 */
void loop() {
  bool flag = false;
  byte data[256];
  
   // Check if data was received from screen
  while (screenSerial.available() ) {
    byte start = readByteFromScreen();
    debugSerial.print("Received byte ");
    debugSerial.println(start);
  
    // If it was a START_BYTE start processing
    if ( start == START_BYTE ) {
      
      // Start reading TARGET_ID, CMD_ID and DLEN
      byte id = readByteFromScreen();
      byte cmd = readByteFromScreen();
      byte dlen = readByteFromScreen();

      readBytesFromScreen(dlen,data);

      // debug Output
      debugSerial.print("id: ");
      debugSerial.print(id);
      debugSerial.print(" cmd: ");
      debugSerial.print(cmd);
      debugSerial.print(" dlen: ");
      mantel.debugData(data,dlen);
      debugSerial.println();


      if ( id == 0 ) {
        // If its for the master, do it yourself
        debugSerial.println("Processing commd myself");
      } else {
        // Otherwise send cmd dlen data to id

        I2c.write(id,cmd,data,dlen+1);
        /*
        Wire.beginTransmission(id);
        Wire.write(cmd);
        Wire.write(dlen);
        for (int i = 0; i < dlen; i++) {
          Wire.write(data[i]);             
        }
        Wire.endTransmission();
        */
        
        debugSerial.println("Relayed data:");
        debugSerial.print(cmd);
        debugSerial.print(" ");
        debugSerial.print(dlen);
        for (int i = 0; i < dlen; i++) {
          debugSerial.print(" ");
          debugSerial.print(data[i]);
        }
        debugSerial.println();
        debugSerial.flush();
      }
    }
  }
  delay(20);
}
