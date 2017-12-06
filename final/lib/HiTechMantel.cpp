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
  byte b = Wire.available() ? Wire.read() : 0;
  return b;
}


void HiTechMantel::readData(int len, byte buf[]) {
  for (int i = 0; i < len; i++ ) {
    buf[i] = readFromWire();
  }
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
