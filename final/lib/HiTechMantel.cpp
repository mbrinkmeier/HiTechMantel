/**
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

#include"Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include "HiTechMantel.h"

HiTechMantel::HiTechMantel() {
  pixel = Adafruit_NeoPixel(1,8,NEO_GRBW + NEO_KHZ800);
  pixel.begin();

}

byte HiTechMantel::readFromWire() {
  byte b = ( Wire.available() > 0 ) ? Wire.read() : 0;
  return b;
}


void HiTechMantel::readData(int len, byte buf[]) {
  for (int i = 0; i < len; i++ ) {
    buf[i] = readFromWire();
  }
}

void HiTechMantel::emptyWire() {
  while (Wire.available()>0) {
    Wire.read();
  }
}

void HiTechMantel::emptyWire(int count) {
  while ( (Wire.available()>0) && (count > 0) ) {
    Wire.read();
    count--;
  }
}


void HiTechMantel::writeByteToSlave(byte id, byte data) {
  Wire.beginTransmission(id);
  Wire.write(data);
  Wire.endTransmission(id);
}


void HiTechMantel::writeByteToSlave(byte id, byte cmd, byte data) {
  Serial.print("Writing ");
  Serial.println(data);
  Wire.beginTransmission(id);
  Wire.write(cmd);
  Wire.write(1);
  Wire.write(data);
  Wire.endTransmission(id);
}


void HiTechMantel::writeBytesToSlave(byte id, byte cmd, byte data[], int dlen) {
  Wire.beginTransmission(id);
  Wire.write(cmd);
  Wire.write(dlen);
  for (int i = 0; i < dlen; i++ ) {
   Wire.write(data[i]);
  }
  Wire.endTransmission(id);
}


byte HiTechMantel::readByteFromSlave(byte id) {
  byte b;
  Serial.print(F("Request from "));
  Serial.println(id);
  Wire.requestFrom((int) id,1,true);

  delay(500);
  if ( Wire.available() > 0 ) {
     b = Wire.read();
     while ( Wire.available() ) {
       Serial.println(Wire.read());
     }

     delay(200);

 } else {
   Serial.println(F("Wire not available"));
   return 0;
 }
  return b;
}


byte HiTechMantel::readByteFromScreen() {
  byte b = screenSerial.available() ? screenSerial.read() : 0;
  delay(1);
  return b;
}


void HiTechMantel::readBytesFromScreen(byte buf[], int len ) {
  for (int i = 0; i < len; i++) {
    buf[i] = readByteFromScreen();
  }
}


void HiTechMantel::writeToScreen(String cmd) {
  screenSerial.print(cmd);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
}


byte HiTechMantel::rainbowRed(int pos, int intervalLength) {
  int intNo = (pos / intervalLength) % 6;
  int intPos = pos % intervalLength;

  switch (intNo) {
    case 0:
    case 5:
      return 255;
    case 1:
      return 255 - intPos*255/intervalLength;
    case 4:
      return intPos*255/intervalLength;
  }
  return 0;
}


byte HiTechMantel::rainbowGreen(int pos, int intervalLength) {
  int intNo = (pos / intervalLength) % 6;
  int intPos = pos % intervalLength;

  switch (intNo) {
    case 1:
    case 2:
      return 255;
    case 3:
      return 255 - intPos*255/intervalLength;
    case 0:
      return intPos*255/intervalLength;
  }
  return 0;
}


byte HiTechMantel::rainbowBlue(int pos, int intervalLength) {
  int intNo = (pos / intervalLength) % 6;
  int intPos = pos % intervalLength;

  switch (intNo) {
    case 3:
    case 4:
      return 255;
    case 5:
      return 255 - intPos*255/intervalLength;
    case 2:
      return intPos*255/intervalLength;
  }
  return 0;
}


void HiTechMantel::debugData(byte buf[], int len) {
  for (int i = 0 ; i < len; i++ ) {
    Serial.print(" ");
    Serial.print(buf[i]);
  }
}
