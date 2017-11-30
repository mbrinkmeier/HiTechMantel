#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

// Id of the Slave
#define HEART_ID 30

#define DATA_PIN 10
#define SHOW_DELAY 100

// Define the matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, DATA_PIN,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_RGBW            + NEO_KHZ800);

char heart_rate = 0;
unsigned long last_tick = 0;
String text;


// Scroll a text over
void scrollText(char red, char green, char blue, String text) {
  matrix.clear();
  matrix.setTextWrap(true);
  matrix.setTextSize(1);
  matrix.setRotation(0);
  for (int8_t x=7; x>=-42; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.setTextColor(matrix.Color(red,green,blue));
    matrix.print(text);
    matrix.show();
    delay(SHOW_DELAY);
  }
}

// Fill the matrix with the given color
void showColor(char red, char green, char blue) {
  matrix.fillScreen(matrix.Color(red,green,blue));
  matrix.show();
  delay(SHOW_DELAY);
}

// Shows a heart beat
void showHeart(char step) {
  matrix.fillScreen(matrix.Color(255-step*25,0,0));
  matrix.show();
  delay(SHOW_DELAY);  
}


// Handle the received bytes
void receiveEvent(int numBytes) {
  char reg;
  char red;
  char green;
  char blue;

  reg = Wire.read();

  // Clear the screen
  switch (reg) {
    case 0 :
      matrix.fillScreen(matrix.Color(0,0,0));
      matrix.show();
      delay(SHOW_DELAY);
      break;
      
    case 1 :
      text = String();
      red = Wire.available() ? Wire.read() : 0;
      green = Wire.available() ? Wire.read() : 0;
      blue = Wire.available() ? Wire.read() : 0;
      while ( Wire.available() ) {
        char c = Wire.read();
        text = text + c;
      }
      scrollText(red,green,blue,text);
      break;

    case 2 :
      heart_rate = Wire.available() ? Wire.read() : 0;
      last_tick = 0;
      break;

    case 3 :
      red = Wire.available() ? Wire.read() : 0;
      green = Wire.available() ? Wire.read() : 0;
      blue = Wire.available() ? Wire.read() : 0;
      showColor(red,green,blue);
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


void loop() {
  // Decide which animation is shown
  if ( heart_rate > 0 ) {
    if (last_tick == 0) {
      last_tick = millis();
      showHeart(0);
    } else {
      unsigned long cur_tick = millis();
      unsigned long delta = cur_tick - last_tick;
      char step = delta * 10 / heart_rate;
      if ( step >= 10 ) {
        last_tick = cur_tick;
        step = 0;
      }
      showHeart(step);
    }
  }
}



