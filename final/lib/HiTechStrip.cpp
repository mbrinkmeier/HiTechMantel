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

#include "HiTechMantel.h"
#include "HiTechStrip.h"
#include <Wire.h>

HiTechStrip* STRIP;

extern void initAniOwn(HiTechStrip *strip);
extern void doAniOwn(HiTechStrip *strip, int frame);

HiTechStrip::HiTechStrip() {
  mantel = new HiTechMantel();
  strip = new Adafruit_NeoPixel(LEN, PIN,  NEO_GRB + NEO_KHZ800);
  STRIP = this;
}


void HiTechStrip::setup() {

  debugSerial.begin(9600);
  delay(500);

  Wire.begin(ID_STRIP);
  Wire.onReceive(handleStripMsg);

  strip->begin();

  // debugSerial.print("Running selftest ... ");
  // selftest();
  // debugSerial.println("finished");

  debugSerial.print("NeoPixelStrip listening as ID ");
  debugSerial.println(ID_STRIP);

  debugSerial.println(F("NeoPixelStrip initialized"));
  debugSerial.flush();

  initAniOwn(this);
  // initAniRunning();
}


void HiTechStrip::loop() {
  unsigned long now = millis();

  if ( (colChanged) || ((now - lastFrame > frameDelay) && (frameDelay > 0)) ) {
    switch ( frameAni ) {
      case ANI_COLOR:
        doAniColor(frameCount);
        break;
      case ANI_RAINBOW:
        doAniRainbow(frameCount);
        break;
      case ANI_PULSE:
        doAniPulse(frameCount);
        break;
      case ANI_OWN:
        doAniOwn(STRIP,frameCount);
        break;
      case ANI_RUNNING:
        doAniRunning(frameCount);
        break;
    }
    colChanged = false;
    lastFrame = now;
    frameCount = ( frameCount + 1 ) % frameNumber;
    strip->show();
    delay(SHOW_DELAY);
  }
}


/**
 * The selftest
 */
void HiTechStrip::selftest() {
  // initAniColor(0,0,0);

  for (int i = 0; i < LEN; i=i+1) {
    strip->setPixelColor(i,255,0,0);
    strip->show();
    delay(5);
    strip->setPixelColor(i,0,0,255);
    strip->show();
    delay(5);
    strip->setPixelColor(i,0,0,0);
    strip->show();
  }
  delay(5);
}



/**
 * Initialize constant color
 */
void HiTechStrip::initAniColor(int red, int green, int blue) {
  // colRed = red;
  // colGreen = green;
  // colBlue = blue;

  frameAni = ANI_COLOR;
  frameCount = 1;
  frameNumber = 1;
  frameDelay = 0;  // No animation
  lastFrame = millis();

  colChanged = true;
}

/**
 * Set everything to the same color
 */
void HiTechStrip::doAniColor(int frame) {
  for (int i = 0; i < LEN; i=i+1) {
    strip->setPixelColor(i,colRed,colGreen,colBlue);
  }
}


/**
 * Initializte Rainbow animation
 */
void HiTechStrip::initAniRainbow() {
  frameAni = ANI_RAINBOW;
  frameCount = 0;
  frameNumber = LEN;
  frameDelay = 100;
}

/**
 * Compute the rainbow frame
 */
void HiTechStrip::doAniRainbow(int frame) {
  int ilen = 21;
  int interval;
  int red = 0;
  int green = 0;
  int blue = 0;

  for ( int pos = 0; pos < LEN; pos = pos+1 ) {
    int lpos = 127 + pos - frame;
    interval = (lpos / ilen) % 6; // Number of interval
    int ipos = lpos % ilen;        // position inside intervall
    switch (interval) {
      case 0:
         red = 255;
         green = (255*ipos)/ilen;
         blue = 0;
         break;
      case 1:
         red = 255 - (255*ipos)/ilen;
         green = 255;
         blue = 0;
         break;
      case 2:
         red = 0;
         green = 255;
         blue = (255*ipos)/ilen;
         break;
      case 3:
         red = 0;
         green = 255 - (255*ipos)/ilen;
         blue = 255;
         break;
      case 4:
         red = 255*ipos/ilen;
         green = 0;
         blue = 255;
         break;
      case 5:
         red = 255;
         green = 0;
         blue = 255 - (255*ipos)/ilen;
         break;
    }
    strip->setPixelColor(pos,red,green,blue);
  }
}


/**
 * Set a pulse of the current color
 */
void HiTechStrip::initAniPulse() {
  frameAni = ANI_PULSE;
  frameCount = 0;
  frameNumber = 10;
  frameDelay = 100;
}


/**
 *
 */
void HiTechStrip::doAniPulse(int frame) {
  for (int i = 0; i < LEN; i++) {
    int pos = (i+frame) % frameNumber;
    int r = (colRed*pos*pos)/(frameNumber*frameNumber);
    int g = (colGreen*pos*pos)/(frameNumber*frameNumber);
    int b = (colBlue*pos*pos)/(frameNumber*frameNumber);
    strip->setPixelColor(i,r,g,b);
  }
}

/**
 * Set a pulse of the current color
 */
void HiTechStrip::initAniRunning() {
  frameAni = ANI_RUNNING;
  frameCount = 0;
  frameNumber = 3;
  frameDelay = 100;
}


/**
 *
 */
void HiTechStrip::doAniRunning(int frame) {
  for (int i = 0; i < LEN; i++) {
    int pos = (i+frame) % 3;
    int r = (pos == 0) ? 255/DIMMER : 0 ;
    int g = (pos == 1) ? 255/DIMMER : 0 ;
    int b = (pos == 2) ? 255/DIMMER : 0 ;
    strip->setPixelColor(i,r,g,b);
  }
}


/**
 * Set the anmiation speed
 */
void HiTechStrip::setSpeed(byte speed) {
  switch (frameAni) {
    case ANI_PULSE:
      frameDelay = 6000/speed;
      break;
    default:
      frameDelay = map(speed,0,255,500,10);
      break;
  }
  // Serial.print("Delay set to ");
  // Serial.println(frameDelay);
}


void HiTechStrip::setColor(int led, long color) {
  byte blue = color % 256;
  color = color / 256;
  byte green = color % 256;
  color = color / 256;
  byte red = color % 256;
  strip->setPixelColor(led,red,green,blue);
  if ( led == 0 ) {
    mantel->pixel.setPixelColor(0,red,green,blue);
    mantel->pixel.show();
  }
}

void HiTechStrip::clearStrip() {
  for (int led = 0; led < LEN; led++) {
    strip->setPixelColor(led,0,0,0);
  }
}

void HiTechStrip::showColors(int start, int len, boolean repeat, boolean reverse) {
  clearStrip();

  int fak = 1;
  if (reverse) fak = -1;

  if ( repeat == false ) {
    for ( int i = 0; i < len ; i++ ) {
      int pos = (LEN + i+fak*start) % LEN;
      setColor(pos,colors[i]);
    }
  } else {
    for ( int i = 0; i < LEN ; i++ ) {
      int pos = (LEN + i+fak*start) % len;
      setColor(i,colors[pos]);
    }
  }
}


long HiTechStrip::colorToLong(int red, int green, int blue) {
  long col = ((long) red * 256 + (long) green) * 256 + (long) blue ;
  return col;
}


long HiTechStrip::mixColors(long col1, long col2) {
  byte blue1 = col1 % 256;
  col1 = col1 / 256;
  byte green1 = col1 % 256;
  col1 = col1 / 256;
  byte red1 = col1 % 256;

  byte blue2 = col2 % 256;
  col2 = col2 / 256;
  byte green2 = col2 % 256;
  col2 = col2 / 256;
  byte red2 = col2 % 256;

  red1 = red2 > red1 ? red2 : red1;
  green1 = green2 > green1 ? green2 : green1;
  blue1 = blue2 > blue1 ? blue2 : blue1;

  return colorToLong(red1,green1,blue1);
}


long HiTechStrip::runningPixel(int pos, int frame, int start, long color, boolean reverse) {
  int fak = 1;
  if ( reverse ) fak = -1;

  int led = (LEN + start + fak * frame) % LEN;

  if ( led == pos ) return color;

  return 0;
}

long HiTechStrip::runningPixels(int pos, int frame, int start, int len, long color[], boolean reverse) {
  int fak = 1;
  if ( reverse ) fak = -1;

  int led = (LEN + start + fak * frame) % LEN;

  int idx = pos - led;
  if ( (idx >= 0) && (idx < len) ) return color[idx];

  return 0;
}

long HiTechStrip::runningRainbow(int pos, int frame, int start, int len, boolean reverse) {
  int fak = 1;
  if ( reverse ) fak = -1;

  int led = (LEN + start + fak * frame) % LEN;

  int idx = pos - led;
  if ( (idx >= 0) && (idx < len) ) return colorToLong(mantel->rainbowRed(idx,len/6),
     mantel->rainbowGreen(idx,len/6),mantel->rainbowBlue(idx,len/6));

  return 0;
}

/**
 * Handle incoming I2C Transmissions
 *
 * Each transmission consists of
 * <cmd> <dlen> <data[0]> ... <data[dlen-1]>
 */
void handleStripMsg(int numBytes) {
  byte cmd = STRIP->mantel->readFromWire();
  byte dlen = STRIP->mantel->readFromWire();
  byte data[255];

  STRIP->mantel->readData(dlen,data);
  STRIP->mantel->emptyWire();

  /*
  debugSerial.print(F("Received cmd: "));
  debugSerial.print(cmd);
  debugSerial.print(F(" dlen: "));
  debugSerial.print(dlen);
  debugSerial.print(F(" "));
  STRIP->mantel->debugData(data,dlen);
  debugSerial.println();
  */
  
  switch (cmd) {
    case CMD_STRIP_RESET:
      STRIP->colRed = 0;
      STRIP->colGreen = 0;
      STRIP->colBlue = 0;
      STRIP->colChanged = true;
      STRIP->initAniColor(0,0,0);
      break;
    case CMD_STRIP_COLOR:
      STRIP->initAniColor(data[0],data[1],data[2]);
      break;
    case CMD_STRIP_RAINBOW:
      STRIP->initAniRainbow();
      break;
    case CMD_STRIP_SPEED:
      STRIP->setSpeed(data[0]);
      break;
    case CMD_STRIP_RED:
      STRIP->colRed = data[0];
      STRIP->colChanged = true;
      break;
    case CMD_STRIP_GREEN:
      STRIP->colGreen = data[0];
      STRIP->colChanged = true;
      break;
    case CMD_STRIP_BLUE:
      STRIP->colBlue = data[0];
      STRIP->colChanged = true;
      break;
    case CMD_STRIP_PULSE:
      STRIP->initAniPulse();
      break;
    case CMD_STRIP_OWN:
      STRIP->frameAni = ANI_OWN;
      initAniOwn(STRIP);
      break;
    case CMD_STRIP_RUNNING:
      STRIP->initAniRunning();
      break;
  }
}
