/*
 * This sketch controls the heart matrix.
 * 
 * The loop displays the current frame and waits a given time
 * till the next frame has to be displayed.
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
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define debugSerial Serial

#define DATA_PIN 9
#define SHOW_DELAY 10
#define DIMMER 3

#define ANI_NONE  0
#define ANI_COLOR  1
#define ANI_TEXT  2
#define ANI_PULSE 3
#define ANI_RAINBOW 4
#define ANI_SWIRL 5
#define ANI_WINDMILL 6
#define ANI_PULSING 7
#define ANI_WAVE 8


volatile int frameAni;     // The id of the animation
volatile int frameDelay;   // The time between two frames
volatile int frameNumber;  // The number of frames for the current animation
volatile int frameCount;   // Counts the current frame
volatile int colRed = 0;
volatile int colGreen = 0;
volatile int colBlue = 100;

unsigned long lastFrame;
String aniText;
volatile int aniSpeed;
bool colChanged = false;

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

// Define the matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, DATA_PIN,
  NEO_MATRIX_TOP  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRBW            + NEO_KHZ800);

// char heart_rate = 0;
unsigned long last_tick = 0;

void (*hardReset)(void) = 0;

/**
 * Initialize serial connections, I2C connection, pixel and strip.
 */
void setup() {
  debugSerial.begin(9600);

  Wire.begin(ID_MATRIX);        // join i2c bus with ID_MATRIX
  Wire.onReceive(handleMsg); // register event

  delay(100);
  matrix.begin();
  matrix.fillScreen(matrix.Color(0,0,0));

  debugSerial.print("NeoPixelMatrix listening as ID ");
  debugSerial.println(ID_MATRIX);
  

  debugSerial.print("Running selftest ... ");
  selftest();
  debugSerial.println("finished");

  // aniSpeed = 255;
  // initAniWave();
  
  initAniText("   Willkommen!",14);
}


/**
 * The main loop
 * 
 * Checks if the time since last frame is longer than  frameDelay.
 * If this is the case, the frameCount is increased and the new
 * frame is computed and displayed.
 */
void loop() {
  long time = millis();
  
  if ( (colChanged) || ((time - lastFrame > frameDelay) && (frameDelay > 0)) ) {
    switch ( frameAni ) {
      case ANI_TEXT:
        doAniText(frameCount);
        break;
      case ANI_COLOR:
        doAniColor(frameCount);
        break;
      case ANI_PULSE:
        doAniPulse(frameCount);
        break;
      case ANI_RAINBOW:
        doAniRainbow(frameCount);
        break;
      case ANI_SWIRL:
        doAniSwirl(frameCount);
        break;
      case ANI_WINDMILL:
        doAniWindmill(frameCount);
        break;
      case ANI_PULSING:
        doAniPulsing(frameCount);
        break;
      case ANI_WAVE:
        doAniWave(frameCount);
        break;
    }
    colChanged = false;
    lastFrame= millis();
    frameCount = ( frameCount + 1 ) % frameNumber; 
    matrix.show();
    delay(SHOW_DELAY);
  }
}


/**
 * Handle incoming I2C Transmissions
 * 
 * Each transmission consists of
 * <cmd> <dlen> <data[0]> ... <data[dlen-1]>
 */
void handleMsg(int numBytes) {
  byte cmd = mantel.readFromWire();
  byte dlen = mantel.readFromWire();
  byte data[255];
  mantel.readData(dlen,data);
  mantel.emptyWire();
    
  // debugSerial.print(F("Received cmd: "));
  // debugSerial.print(cmd);
  // debugSerial.print(F(" dlen: "));
  // debugSerial.println(dlen);
  // mantel.debugData(data,dlen);
  // debugSerial.println();
  
  switch (cmd) {
    case CMD_MATRIX_RESET:
      initAniColor(0,0,0);
      break;
    case CMD_MATRIX_COLOR:
      initAniColor(colRed,colGreen,colBlue);
      break;
    case CMD_MATRIX_TEXT:
      initAniText(data,dlen);
      break;
    case CMD_MATRIX_SPEED:
      setSpeed(data[0]);
      break;
    case CMD_MATRIX_RED:
      colRed = data[0];
      colChanged = true;
      break;
    case CMD_MATRIX_GREEN:
      colGreen = data[0];
      colChanged = true;
      break;
    case CMD_MATRIX_BLUE:
      colBlue = data[0];
      colChanged = true;
      break;
    case CMD_MATRIX_PULSE:
      initAniPulse();
      break;
    case CMD_MATRIX_RAINBOW:
      initAniRainbow();
      break;
    case CMD_MATRIX_SWIRL:
      initAniSwirl();
      break;
    case CMD_MATRIX_WINDMILL:
      initAniWindmill();
      break;
    case CMD_MATRIX_PULSING:
      initAniPulsing();
      break;
    case CMD_MATRIX_WAVE:
      initAniWave();
      break;
    case CMD_HARD_RESET:
      hardReset();
      break;
  }
}



/* 
 * The selftest 
 */
void selftest() {
  // Do a little self test
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      matrix.drawPixel(row,col,matrix.Color(255,0,0));
      matrix.show();
      delay(10);
      matrix.drawPixel(row,col,matrix.Color(0,255,0));
      matrix.show();
      delay(10);
      matrix.drawPixel(row,col,matrix.Color(0,0,255));
      matrix.show();
      delay(10);
      matrix.drawPixel(row,col,matrix.Color(0,0,0));
    }
  }
  matrix.show();
}


/**
 * Initialize constant color
 */
void initAniColor(int red, int green, int blue) {
  colRed = red;
  colGreen = green;
  colBlue = blue;
  colChanged = true;
  
  frameAni = ANI_COLOR;
  frameCount = 1;
  frameNumber = 1;
  frameDelay = 0;  // No animation
  lastFrame = millis();
}

/**
 * Do the color
 */
void doAniColor(int frame) {
  matrix.fillScreen(matrix.Color(colRed/DIMMER,colGreen/DIMMER,colBlue/DIMMER));
}


/**
 * Initialize scrolling text
 * 
 * Th first byte contains the speed
 * The next three bytes of data contain the color.
 * The remaining dlen-4 bytes contain the text.
 */
void initAniText(byte data[], int dlen) {
  frameAni = ANI_TEXT;
  setSpeed(127);
  frameNumber = (dlen-3) * 8;
  frameCount = 0;
  aniText = "";
  for ( int i = 0; i < dlen-3; i++) {
    aniText = aniText + (char) data[i+3];
  }

  // debugSerial.print("text :");
  // debugSerial.println(aniText);
}


/**
 * Compute the next frame for scrolling text
 */
void doAniText(int frame) {
  int cursor = 10-frame;
  matrix.fillScreen(0);
  matrix.setTextColor(matrix.Color(colRed,colGreen,colBlue));
  matrix.setTextWrap(false);
  matrix.setCursor(6-frameCount,0);
  matrix.print(aniText);
  // setSpeed(aniSpeed);
}


/**-
 * Initialize the Pulse animation
 */
void initAniPulse() {
  frameAni = ANI_PULSE;
  frameNumber = 3;
  frameCount = 0;
  aniText = "";
  if ( (colRed == 0) && ( colGreen == 0) && (colBlue == 0) ) {
    colRed = 127;
  }
  colChanged = true;
  setSpeed(70);
}

// int heart[64] = {
// }

byte hearAni[3][8][8] = {
  {
    {0,1,1,0,0,1,1,0},
    {1,0,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,1,0},
    {0,0,1,0,0,1,0,0},
    {0,0,0,1,1,0,0,0}    
  }
  ,
  {
    {0,1,1,0,0,1,1,0},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,1,1,0,0,0}
  }
  ,
  {
    {0,1,1,0,0,1,1,0},
    {1,0,0,1,1,0,0,1},
    {1,0,1,0,0,1,0,1},
    {1,0,1,1,1,1,0,1},
    {1,0,1,1,1,1,0,1},
    {0,1,0,1,1,0,1,0},
    {0,0,1,0,0,1,0,0},
    {0,0,0,1,1,0,0,0}
  }
};

/**
 * Compute the frame for pulse animation.
 * 
 * It pulses with the set frequency
 */
void doAniPulse(int frame) {
  int brightness;
  
  int radius = 128 * ( frameNumber - frame ) / frameNumber;
  
  int dist;
  for ( int col = 0; col < 8; col++ ) {    
    for ( int row = 0; row < 8; row++ ) {
      /*
      dist = (2*col-7)*(2*col-7) +(2*row-7)*(2*row-7); // distance from center
      if ( dist < radius ) {
        brightness = 255 * ( frameNumber - frame ) / frameNumber;
      } else {
        brightness = 255 * ( frameNumber - frame ) / (frameNumber*2);
      }
      */
      brightness = hearAni[frame][row][col]*255;
      matrix.drawPixel(col,row,matrix.Color((colRed*brightness/255)/DIMMER,(colGreen*brightness/255)/DIMMER,(colBlue*brightness/255)/DIMMER));
    }
  }
}


/**
 * Initialize the rainbow animation
 */
void initAniRainbow() {
  frameAni = ANI_RAINBOW;
  frameNumber = 30;
  frameCount = 0;
  aniText = "";
  setSpeed(aniSpeed);
}


/**
 * Compute the frame for rainbow animation.
 */
void doAniRainbow(int frame) {
  int red, green, blue;
  int pos;
  for ( int col = 0; col < 8; col++ ) {
    for ( int row = 0; row < 8; row++ ) {    
      pos = (col+row+frame) % frameNumber;
      red = mantel.rainbowRed(pos,frameNumber/6)/DIMMER;
      green = mantel.rainbowGreen(pos,frameNumber/6)/DIMMER;
      blue = mantel.rainbowBlue(pos,frameNumber/6)/DIMMER;
      matrix.drawPixel(col,row,matrix.Color(red/DIMMER,green/DIMMER,blue/DIMMER));
    }
  }
}

/**
 * Initialize the swirl animation
 */
void initAniSwirl() {
  frameAni = ANI_SWIRL;
  frameNumber = 64;
  frameCount = 0;
  aniText = "";
  setSpeed(aniSpeed);
}


/**
 * Compute the frame for swirl animation.
 */
void doAniSwirl(int frame) {
  int red, green, blue;
  int col,row,dcol,drow,dummy;
  col = 0;
  row = 0;
  dcol = 1;
  drow = 0;
  for (int pos = 0; pos < 64; pos ++ ) {
    int ipos = (64+pos-frame)%64;
    red = mantel.rainbowRed(ipos,11)/DIMMER;
    green = mantel.rainbowGreen(ipos,11)/DIMMER;
    blue = mantel.rainbowBlue(ipos,11)/DIMMER;
    matrix.drawPixel(col,row,matrix.Color(red,green,blue));
    col = col + dcol;
    row = row + drow;
    if ( ( (row==col+1) && (col <= 3) ) || ( (row == col ) && (col>3) ) )  {
      dummy = dcol;
      dcol = -drow;
      drow = dummy; 
    } else if ( row == 7-col ) {
      dummy = dcol;
      dcol = drow;
      drow = dummy;
    }
  }

}

/**
 * Initialize the windmill animation
 */
void initAniWindmill() {
  frameAni = ANI_WINDMILL;
  frameNumber = 7;
  frameCount = 0;
  aniText = "";
  setSpeed(aniSpeed);
}


/**
 * Compute the frame for windmill animation.
 */
void doAniWindmill(int frame) {
  int bright,red,green,blue;
  matrix.clear();
  for (int i = 1; i >= 0 ; i--) {
    bright = (i==0) ? 255 : 64;
    red = map(colRed,0,255,0,bright);
    green = map(colGreen,0,255,0,bright);
    blue = map(colBlue,0,255,0,bright);
    matrix.drawLine(frame-i,0,7-frame+i,7,matrix.Color(red,green,blue));
    matrix.drawLine(0,7-frame+i,7,frame-i,matrix.Color(red,green,blue));
  }
}


/**
 * Initialize the circle animation
 */
void initAniPulsing() {
  frameAni = ANI_PULSING;
  frameNumber = 8;
  frameCount = 0;
  aniText = "";
  setSpeed(aniSpeed);
}


/**
 * Compute the frame for circles animation.
 */
void doAniPulsing(int frame) {
  int bright,red,green,blue;
  matrix.clear();
  for (int i = 0; i < 5 ; i++ ) {
    bright = map((i+frame) % frameNumber,0,frameNumber-1,0,255);
    red = map(colRed,0,255,0,bright)/2;
    green = map(colGreen,0,255,0,bright)/2;
    blue = map(colBlue,0,255,0,bright)/2;
    matrix.fillRoundRect(3-i,3-i,2*(i+1),2*(i+1),i,matrix.Color(red,green,blue));
  }
}

/**
 * Initialize the circle animation
 */
void initAniWave() {
  frameAni = ANI_WAVE;
  frameNumber = 16;
  frameCount = 0;
  aniText = "";
  setSpeed(aniSpeed);
}


/**
 * Compute the frame for circles animation.
 */
void doAniWave(int frame) {
  int red, green, blue;
  int pos, bright;
  for ( int col = 0; col < 8; col++ ) {
    for ( int row = 0; row < 8; row++ ) {    
      pos = (col+row+frame) % frameNumber;
      bright = map(pos,0,frameNumber-1,255,48);
      red = map(colRed,0,255,0,bright);
      green = map(colGreen,0,255,0,bright);
      blue = map(colBlue,0,255,0,bright);
      matrix.drawPixel(col,row,matrix.Color(red/2,green/2,blue/2));
    }
  }
}


/**
 * Set the animation speed
 */
void setSpeed(int speed) {
  aniSpeed = speed;
  switch (frameAni) {
    case ANI_PULSE:
      // speed is BPM
      frameDelay = 60000 / (aniSpeed*frameNumber);
      break;
    case ANI_RAINBOW:
      frameDelay = map(aniSpeed,0,255,200,5);
      break;
    case ANI_WINDMILL:
      frameDelay = map(aniSpeed,0,255,200,50); 
      break;
    case ANI_SWIRL:
      frameDelay = map(aniSpeed,0,255,200,5); 
      break;
    case ANI_TEXT:
      frameDelay = map(aniSpeed,0,255,200,50);
      break;
    default:
      frameDelay = map(aniSpeed,0,255,500,5);
  }
  // Serial.print(F("Set frame delay to "));
  // Serial.print(frameDelay);
  // Serial.println(F(" ms"));
}
