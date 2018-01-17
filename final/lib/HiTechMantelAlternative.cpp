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
#include <I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include "HiTechMantelAlternative.h"

HiTechMantelAlternative::HiTechMantelAlternative() {
  pixel = Adafruit_NeoPixel(1,8,NEO_GRBW + NEO_KHZ800);
  pixel.begin();

}

byte HiTechMantelAlternative::readFromWire() {
  byte b = ( I2c.available() > 0 ) ? I2c.receive() : 0;
  return b;
}


void HiTechMantelAlternative::readData(int len, byte buf[]) {
  for (int i = 0; i < len; i++ ) {
    buf[i] = readFromWire();
  }
}

void HiTechMantelAlternative::emptyWire() {
  while (I2c.available()>0) {
    I2c.receive();
  }
}

void HiTechMantelAlternative::emptyWire(int count) {
  while ( (I2c.available()>0) && (count > 0) ) {
    I2c.receive();
    count--;
  }
}


/**
 * Master writes to I2C slave
 */
void HiTechMantelAlternative::writeByteToSlave(byte id, byte data) {
  debugSerial.println(F("Writing starts"));
  int code = I2c.write(id,data);
  if ( code != 0 ) {
    debugSerial.print(F("Error writing to slave "));
    debugSerial.print(id);
    debugSerial.print(F(": Code "));
    debugSerial.println(code);
  }
  debugSerial.println(F("Writing ends"));
  debugSerial.flush();
}


void HiTechMantelAlternative::writeByteToSlave(byte id, byte cmd, byte data) {
  debugSerial.println(F("Writing starts"));
  byte buf[2];
  buf[0] = 1;
  buf[1] = data;
  int code = I2c.write(id,cmd,buf,2);
  if ( code != 0 ) {
    debugSerial.print(F("Error writing to slave "));
    debugSerial.print(id);
    debugSerial.print(F(" : Code "));
    debugSerial.println(code);
  }
  debugSerial.println(F("Writing ends"));
  debugSerial.flush();
}


void HiTechMantelAlternative::writeBytesToSlave(byte id, byte cmd, byte data[], int dlen) {
  debugSerial.println(F("Writing starts"));
  byte buf[dlen+1];
  buf[0] = dlen;
  for (int i = 0; i < dlen; i++ ) {
    buf[i+1] = data[i];
  }
  int code = I2c.write(id,cmd,buf,dlen+1);
  if ( code != 0 ) {
    debugSerial.print(F("Error writing to slave "));
    debugSerial.print(id);
    debugSerial.print(F(" : Code "));
    debugSerial.println(code);
  }
  debugSerial.println(F("Writing ends"));
  debugSerial.flush();
}

/**
 * Read a byte from an I2C slave
 */
byte HiTechMantelAlternative::readByteFromSlave(byte id) {
  byte b;
  int code = I2c.read(id,1);
  if ( code != 0 ) {
    debugSerial.print(F("Error reading from slave "));
    debugSerial.print(id);
    debugSerial.print(F(" : Code "));
    debugSerial.println(code);
  }
  b = I2c.receive();
  delay(3);
  return b;
}


/**
 * Reading and writing to the screen
 */
byte HiTechMantelAlternative::readByteFromScreen() {
  byte b = screenSerial.available() ? screenSerial.read() : 0;
  delay(1);
  return b;
}


void HiTechMantelAlternative::readBytesFromScreen(byte buf[], int len ) {
  for (int i = 0; i < len; i++) {
    buf[i] = readByteFromScreen();
  }
}


void HiTechMantelAlternative::writeToScreen(String cmd) {
  screenSerial.print(cmd);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.flush();
}

/**
 * Rainbow colors
 */

byte HiTechMantelAlternative::rainbowRed(int pos, int intervalLength) {
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


byte HiTechMantelAlternative::rainbowGreen(int pos, int intervalLength) {
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


byte HiTechMantelAlternative::rainbowBlue(int pos, int intervalLength) {
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

/**
 * Debugging
 */

void HiTechMantelAlternative::debugData(byte buf[], int len) {
  for (int i = 0 ; i < len; i++ ) {
    Serial.print(" ");
    Serial.print(buf[i]);
  }
}
