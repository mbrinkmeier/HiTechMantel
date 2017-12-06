// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Hallo' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 27


Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_RGBW            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.clear();
  matrix.show();
  delay(1000);
  Serial.println("Initilization done");
//   matrix.setTextWrap(false);
//  matrix.setBrightness(10);
//  matrix.setTextColor(colors[0]);
//  matrix.fillScreen(matrix.Color(0,255,0));
}

int x    = matrix.width();
int pass = 0;

void loop() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      Serial.print(row);
      Serial.print(" ");
      Serial.println(col);
      matrix.drawPixel(row,col,matrix.Color(255,0,0));
      matrix.show();
      delay(100);
      matrix.drawPixel(row,col,matrix.Color(00,0,0));
    }
  }
/*  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(F("H"));
  if(--x < -36) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(1000);*/
}
