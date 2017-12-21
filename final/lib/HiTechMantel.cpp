#include"Arduino.h"
#include "HiTechMantel.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

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
  Wire.beginTransmission(id);
  Wire.write(cmd);
  Wire.write(1);
  Wire.write(data);
  Wire.endTransmission(id);
}


void HiTechMantel::writeBytesToSlave(byte id, byte cmd, byte data[], int dlen) {
  Wire.beginTransmission(id);
  Wire.write(cmd);
  for (int i = 0; i < dlen; i++ ) {
   Wire.write(data[i]);
  }
  Wire.endTransmission(id);
}


byte HiTechMantel::readByteFromSlave(byte id) {
  byte b;
  Wire.requestFrom(id,1);
  if ( Wire.available() ) {
   b = Wire.read();
   while ( Wire.available() ) Wire.read();
 } else {
   return 0;
 }
  return b;
}


byte HiTechMantel::readByteFromScreen() {
  byte b = Serial1.available() ? Serial1.read() : 0;
  delay(1);
  return b;
}


void HiTechMantel::readBytesFromScreen(byte buf[], int len ) {
  buf[0] = len;
  for (int i = 1; i <= len; i++) {
    buf[i] = readByteFromScreen();
  }
}

void HiTechMantel::writeToScreen(String cmd) {
  screenSerial.print(cmd);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
}

/*
byte HiTechMantel:rainbowRed(int pos, int intervalLength) {
  int intNo = (pos / intervalLength) % 6;
  int intPos = pos % intervalLength;

  switch (intNo) {
    case 0:
    case 5:
      return 255;
    case 1:
      return 255 - intPos*255/intervalLength;
    case 2:
    case 3:
      return 0;
    case 4:
      return intPos*255/intervalLength;
  }
}


byte HiTechMantel:rainbowGreen(int pos, int intervalLength) {
  int intNo = (pos / intervalLength) % 6;
  int intPos = pos % intervalLength;

  switch (intNo) {
    case 1:
    case 2:
      return 255;
    case 3:
      return 255 - intPos*255/intervalLength;
    case 4:
    case 5:
      return 0;
    case 1:
      return intPos*255/intervalLength;
  }
}


byte HiTechMantel:rainbowBlue(int pos, int intervalLength) {
  int intNo = (pos / intervalLength) % 6;
  int intPos = pos % intervalLength;

  switch (intNo) {
    case 3:
    case 4:
      return 255;
    case 5:
      return 255 - intPos*255/intervalLength;
    case 0:
    case 1:
      return 0;
    case 2:
      return intPos*255/intervalLength;
  }
}
*/

void HiTechMantel::debugData(byte buf[], int len) {
  for (int i = 0 ; i < len; i++ ) {
    Serial.print(" ");
    Serial.print(buf[i]);
  }
}
