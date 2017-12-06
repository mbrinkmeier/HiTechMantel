/**
 * This sketch is the programm for the NeoPixel running light strip.
 */
#include <Wire.h>
#include <HiTechMantel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#ifndef PSTR
  #define PSTR // Make Arduino Due happy
#endif

#define debugSerial Serial

#define PIN 9
#define LEN 127
#define SHOW_DELAY 10

#define ANI_COLOR 0
#define ANI_RAINBOW 1

int frameAni;     // The id of the animation
int frameDelay;   // The time between two frames
int frameNumber;  // The number of frames for the current animation
int frameCount;   // Counts the current frame
int colRed;
int colGreen;
int colBlue;
int lastFrame;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEN, PIN,  NEO_GRB           + NEO_KHZ800);

 
void setup() {
  debugSerial.begin(9600);

  Wire.begin(ID_STRIP);
  Wire.onReceive(handleMsg);

  delay(100);
  strip.begin();
  
  debugSerial.print("NeoPixelStrip listening as ID ");
  debugSerial.println(ID_STRIP);
  

  debugSerial.print("Running selftest ... ");
  selftest();
  debugSerial.println("finished");

  initAniRainbow();
}


void loop() {
  long time = millis();
  if ( (time - lastFrame > frameDelay) && (frameDelay > 0) ) {
    switch ( frameAni ) {
      case ANI_RAINBOW:
        doAniRainbow(frameCount);
        break;
    }
    frameCount = ( frameCount + 1 ) % frameNumber; 
    strip.show();
    delay(SHOW_DELAY);
  }
}



void handleMsg(int numBytes) {
  byte cmd = Wire.available() ? Wire.read(): 0;
  byte count = Wire.available() ? Wire.read(): 0;
  byte red;
  byte green;
  byte blue;
  
  debugSerial.print("Received cmd: ");
  debugSerial.print(cmd);
  debugSerial.print("and count: ");
  debugSerial.println(count);

  
  switch (cmd) {
    case CMD_STRIP_RESET:
      initAniColor(0,0,0);
      break;
    case CMD_STRIP_COLOR:
      // Read the next three bytes
      red = Wire.available() ? Wire.read() : 0;
      green = Wire.available() ? Wire.read() : 0;
      blue = Wire.available() ? Wire.read() : 0;
      debugSerial.print("Setting color ");
      debugSerial.print(red);
      debugSerial.print(" ");
      debugSerial.print(green);
      debugSerial.print(" ");
      debugSerial.println(blue);
      initAniColor(red,green,blue);
      break;
    case CMD_STRIP_RAINBOW:
      initAniRainbow();
      break;
  }
  // empty buffer
  while (Wire.available()) Wire.read();
}



void selftest() {
  initAniColor(0,255,0);
  
  for (int i = 0; i < LEN; i=i+1) {
    strip.setPixelColor(i,255,0,0);
    strip.show();
    delay(10);
    strip.setPixelColor(i,0,0,255);
    strip.show();
    delay(10);
    strip.setPixelColor(i,0,0,0);
    strip.show();
  }
  delay(10);
}


void initAniColor(int red, int green, int blue) {
  frameAni = ANI_COLOR;
  frameCount = 1;
  frameNumber = 1;
  frameDelay = 0;  // No animation
  lastFrame = millis();

  for (int i = 0; i < LEN; i=i+1) {
    strip.setPixelColor(i,red,green,blue);
  }
  strip.show();
  delay(SHOW_DELAY);
}

void initAniRainbow() {
  frameAni = ANI_RAINBOW;
  frameCount = 0;
  frameNumber = LEN;
  frameDelay = 100;
  lastFrame = millis();
}


void doAniRainbow(int frame) {
  int interval;
  int ilen = LEN/6;
  byte red;
  byte green;
  byte blue;
  
  for ( int pos = 0; pos < LEN; pos = pos+1 ) {
    int lpos = pos + frame;
    interval = lpos/ilen;
    switch (interval) {
      case 0:
         red = 255;
         green = 255*lpos/ilen;
         blue = 0;
         break;
      case 1:
         red = 255 - (255*(lpos-ilen))/ilen;
         green = 255;
         blue = 0;
         break;
      case 2:
         red = 0;
         green = 255;
         blue = ((lpos-2*ilen)*255)/ilen;
         break;
      case 3:
         red = 0;
         green = 255 - (255*(lpos-3*ilen))/ilen;
         blue = 255;
         break;
      case 4:
         red = (255*(lpos-4*ilen))/ilen;
         green = 0;
         blue = 255;
         break;
      case 5:
         red = 255;
         green = 0;
         blue = 255 - (255*(lpos-5*ilen))/ilen;
         break;
      case 6:
         red=255;
         green = 0;
         blue = 0;
    }
    strip.setPixelColor(pos,red,green,blue);
  }
}

