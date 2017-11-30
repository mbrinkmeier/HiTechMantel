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

#define DATA_PIN 10
#define SHOW_DELAY 100

#define ANI_NONE  0
#define ANI_TEXT  1
#define ANI_HEART 2

int animation;  // The tpe of animation to be displayed
int frame;      // The number of the current frame
int noFrames;   // The number of frames in one cycle
int rate;       // The number of millisecods between frames.
String text;
char red;
char green;
char blue;


// Define the matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, DATA_PIN,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_RGBW            + NEO_KHZ800);

char heart_rate = 0;
unsigned long last_tick = 0;


/*
 * Reset
 */
void reset() {
  matrix.clear();
  matrix.show();
  delay(SHOW_DELAY);  
}


/* 
 * Selftest 
 */
void selftest() {
  // Do a little self test
  for (int x=0; x <8; x=x+1) {
    for (int y = 0; y <8; y=y+1) {
      matrix.drawPixel(x,y,0x00ff00);
      matrix.show();
      delay(SHOW_DELAY);  
      matrix.drawPixel(x,y,0x000000);
      matrix.show();
      delay(SHOW_DELAY);  
    }
  }
}


/* 
 * Fill the matrix with the given color 
 */
void showColor(char red, char green, char blue) {
  animation = ANI_NONE;
  rate = 100;
  frame = 0;
  noFrames = 1;
  
  matrix.fillScreen(matrix.Color(red,green,blue));
  matrix.show();
  delay(SHOW_DELAY);
}


/* 
 * Scroll a text 
 */
void scrollText() {
  matrix.clear();
  matrix.setTextWrap(true);
  matrix.setTextSize(1);
  matrix.setRotation(0);
  int8_t x = 7 + frame;
  matrix.clear();
  matrix.setCursor(x,0);
  matrix.setTextColor(matrix.Color(red,green,blue));
  matrix.print(text);
  matrix.show();
}


/*
 * Shows a heart beat
 */
void heartBeat() {
  matrix.fillScreen(matrix.Color(255-frame*25,0,0));
  matrix.show();
  delay(SHOW_DELAY);  
}


/* 
 * Handle the received bytes 
 */
void receiveEvent(int numBytes) {
  char reg;

  reg = Wire.read();

  // Clear the screen
  switch (reg) {
    case RESET_REG:
      break;
      
    case SELFTEST_REG:
      selftest();
      break;
      
    case HRT_COL_REG :
      text = String();
      red = Wire.available() ? Wire.read() : 0;
      green = Wire.available() ? Wire.read() : 0;
      blue = Wire.available() ? Wire.read() : 0;
      showColor(red,green,blue);
      break;
      
    case HRT_TXT_REG :
      red = Wire.available() ? Wire.read() : 0;
      green = Wire.available() ? Wire.read() : 0;
      blue = Wire.available() ? Wire.read() : 0;
      while ( Wire.available() ) {
        char c = Wire.read();
        text = text + c;
      }
      noFrames = text.length()*8;
      frame = 0;
      rate = 100;
      break;


    case HRT_HRT_REG :
      char rateHi = Wire.available() ? Wire.read() : 0;
      char rateLo = Wire.available() ? Wire.read() : 0;
      rate = (rateHi * 256 + rateLo)/10;
      frame = 0;
      noFrames = 10;
      break;
  }
  // Ignore other registers
}


// Initialize I2C and the matrix
void setup() {
  Wire.begin(HEART_ID);         // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  // Set the matrix up
  matrix.begin();
  matrix.fillScreen(matrix.Color(0,0,0));
  
}


void loop() {
  // Show the next frame
  switch ( animation ) {
    case ANI_TEXT:
      scrollText();
      break;
    case ANI_HEART:
      heartBeat();
      break;
    case ANI_NONE:
      break;
  }
  // Step forward and wait
  frame = (frame + 1) % noFrames;
  delay(rate);
}

