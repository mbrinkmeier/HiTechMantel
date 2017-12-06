// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Hallo' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 9
#define LEN 127


// Example for NeoPixel Shield.  In this application we'd like to use it0x000000
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEN, PIN,  NEO_GRB           + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
  for (int pixel = 0; pixel < LEN; pixel++) {
    strip.setPixelColor(pixel,0,255,0);
  }
}


void loop() {

  for ( int x = 0; x < LEN; x++ ) {
      strip.setPixelColor(x,255,0,0);
      strip.show();
      delay(50);
      strip.setPixelColor(x,0,255,0);
      strip.show();      
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
