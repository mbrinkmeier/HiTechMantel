/*
 * This sketch controls the heart matrix.
 * 
 * The loop displays the current frame and waits a given time
 * till the next frame has to be displayed.
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

#define ANI_NONE  0
#define ANI_COLOR  1
#define ANI_TEXT  2
#define ANI_HEART 3

int frameAni;     // The id of the animation
int frameDelay;   // The time between two frames
int frameNumber;  // The number of frames for the current animation
int frameCount;   // Counts the current frame
int colRed = 0;
int colGreen = 0;
int colBlue = 100;
unsigned long lastFrame;
String aniText;
bool colChanged = false;

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

// Define the matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, DATA_PIN,
  NEO_MATRIX_TOP  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_RGBW            + NEO_KHZ800);

char heart_rate = 0;
unsigned long last_tick = 0;


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
  // selftest();
  debugSerial.println("finished");

  frameAni = ANI_TEXT;
  frameDelay = 200; // Compute delay; 255 should be 25 frames/sec; 0 shold be a second
  frameNumber = 12 * 6;
  frameCount = 0;
  aniText = "Willkommen!";

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
      case ANI_HEART:
        break;
      case ANI_COLOR:
        doAniColor(frameCount);
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
    
  debugSerial.print("Received cmd: ");
  debugSerial.print(cmd);
  debugSerial.print("and dlen: ");
  debugSerial.println(dlen);
  mantel.debugData(data,dlen);
  debugSerial.println();
  
  switch (cmd) {
    case CMD_MATRIX_RESET:
      initAniColor(0,0,0);
      break;
    case CMD_MATRIX_COLOR:
      initAniColor(data[0],data[1],data[2]);
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
  }
  // empty buffer
  while (Wire.available()) Wire.read();
}



/* 
 * The selftest 
 */
void selftest() {
  // Do a little self test
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      Serial.print(row);
      Serial.print(" ");
      Serial.println(col);
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
  matrix.fillScreen(matrix.Color(colRed,colGreen,colBlue));
  matrix.show();
  delay(SHOW_DELAY);
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
  frameDelay = ( 500 - 9 * data[0]/5); // Compute delay; 255 should be 25 frames/sec; 0 shold be a second
  frameNumber = (dlen-3) * 8;
  frameCount = 0;
  aniText = "";
  for ( int i = 0; i < dlen-4; i++) {
    aniText = aniText + (char) data[i+4];
  }
  colRed = data[1];
  colGreen = data[2];
  colBlue = data[3];
  colChanged = true;
  
  debugSerial.print("speed: ");
  debugSerial.print(data[0]);
  debugSerial.print(" red: ");
  debugSerial.print(data[1]);
  debugSerial.print(" green: ");
  debugSerial.print(data[2]);
  debugSerial.print(" blue: ");
  debugSerial.print(data[4]);
  debugSerial.print(" text: ");
  debugSerial.println(aniText);
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
}

/**
 * Set the animation speed
 */
void setSpeed(byte speed) {
  switch (frameAni) {
    case ANI_TEXT:
      frameDelay = ( 500 - 9 * speed/5);
      break;
  }
}

