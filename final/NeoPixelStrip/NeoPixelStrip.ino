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
unsigned long lastFrame;

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

// The strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEN, PIN,  NEO_GRB + NEO_KHZ800);


/**
 * Initialize serial connections, I2C connection, pixel and strip.
 */
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
  if ( (time - lastFrame > frameDelay) && (frameDelay > 0) ) {
    switch ( frameAni ) {
      case ANI_RAINBOW:
        doAniRainbow(frameCount);
        break;
    }
    lastFrame = millis();
    frameCount = ( frameCount + 1 ) % frameNumber; 
    strip.show();
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
  
  debugSerial.print("Received cmd: ");
  debugSerial.print(cmd);
  debugSerial.print("and dlen: ");
  debugSerial.println(dlen);
  mantel.debugData(data,dlen);
  debugSerial.println();
  
  switch (cmd) {
    case CMD_STRIP_RESET:
      initAniColor(0,0,0);
      break;
    case CMD_STRIP_COLOR:
      initAniColor(data[0],data[1],data[2]);
      break;
    case CMD_STRIP_RAINBOW:
      initAniRainbow();
      break;
    case CMD_STRIP_SPEED:
      setSpeed(data[0]);
  }
  // empty buffer
  while (Wire.available()) Wire.read();
}


/**
 * The selftest
 */
void selftest() {
  initAniColor(0,255,0);
  
  for (int i = 0; i < LEN; i=i+1) {
    strip.setPixelColor(i,255,0,0);
    strip.show();
    delay(5);
    strip.setPixelColor(i,0,0,255);
    strip.show();
    delay(5);
    strip.setPixelColor(i,0,0,0);
    strip.show();
  }
  delay(5);
}

/**
 * Initialize constant color
 */
void initAniColor(int red, int green, int blue) {
  frameAni = ANI_COLOR;
  frameCount = 1;
  frameNumber = 1;
  frameDelay = 0;  // No animation
  lastFrame = millis();

  debugSerial.print("Setting color ");
  debugSerial.print(red);
  debugSerial.print(" ");
  debugSerial.print(green);
  debugSerial.print(" ");
  debugSerial.println(blue);

  for (int i = 0; i < LEN; i=i+1) {
    strip.setPixelColor(i,red,green,blue);
  }
  strip.show();
  delay(SHOW_DELAY);
}

/**
 * Initializte Rainbow animation
 */
void initAniRainbow() {
  frameAni = ANI_RAINBOW;
  frameCount = 0;
  frameNumber = LEN;
  frameDelay = 100;
}

/**
 * Compute the rainbow frame
 */
void doAniRainbow(int frame) {
  int ilen = 21;
  int interval;
  int red;
  int green;
  int blue;
  
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
    strip.setPixelColor(pos,red,green,blue);
  }
}



/**
 * Set the anmiation speed
 */
void setSpeed(byte speed) {
  switch (frameAni) {
    case ANI_RAINBOW:
      frameDelay = 490 * (100-speed)/100 + 10;
      break;
  }
  Serial.print("Delay set to ");
  Serial.println(frameDelay);
}

