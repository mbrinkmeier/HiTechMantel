/*********************************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  MIT license, check LICENSE for more information
  All text above, and the splash screen below must be included in
  any redistribution
*********************************************************************/
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
/*=========================================================================
    APPLICATION SETTINGS

    MATRIX DECLARATION        Parameter 1 = width of NeoPixel matrix
                              Parameter 2 = height of matrix
                              Parameter 3 = pin number (most are valid)
                              Parameter 4 = matrix layout flags, add together as needed:
    NEO_MATRIX_TOP,
    NEO_MATRIX_BOTTOM,
    NEO_MATRIX_LEFT,
    NEO_MATRIX_RIGHT          Position of the FIRST LED in the matrix; pick two, e.g.
                              NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
                              NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
                              rows or in vertical columns, respectively; pick one or the other.
    NEO_MATRIX_PROGRESSIVE,
    NEO_MATRIX_ZIGZAG         all rows/columns proceed in the same order,
                              or alternate lines reverse direction; pick one.

                              See example below for these values in action.

    Parameter 5 = pixel type flags, add together as needed:

    NEO_KHZ800                800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    NEO_KHZ400                400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    NEO_GRB                   Pixels are wired for GRB bitstream (most NeoPixel products)
    NEO_RGB                   Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
    -----------------------------------------------------------------------*/
#define FACTORYRESET_ENABLE     1

#define PIN                     9   // Which pin on the Arduino is connected to the NeoPixels?

int led = 7;

// Example for NeoPixel 8x8 Matrix.  In this application we'd like to use it
// with the back text positioned along the bottom edge.
// When held that way, the first pixel is at the top left, and
// lines are arranged in columns, zigzag order.  The 8x8 matrix uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
                            NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRBW            + NEO_KHZ800);
/*=========================================================================*/

void setup(void)
{
  pinMode(led, OUTPUT);
  
  matrix.begin();
  matrix.setBrightness(50);

  matrix.fillScreen(0);
  matrix.show(); // This sends the updated pixel colors to the hardware.
}

void loop(void)
{
  uint16_t c = matrix.Color(0, 255, 0); // Farbe festlegen

  digitalWrite(led, HIGH);     

  matrix.fillScreen(0);
  Circle1(c);
  matrix.show(); // This sends the updated pixel colors to the hardware.

  delay(200);

  matrix.fillScreen(0);
  Circle1(c);
  Circle2(c);
  matrix.show(); // This sends the updated pixel colors to the hardware.

  delay(200);

  digitalWrite(led, LOW);

  matrix.fillScreen(0);
  Circle2(c);
  Circle3(c);
  matrix.show(); // This sends the updated pixel colors to the hardware.

  delay(200);

  matrix.fillScreen(0);
  Circle3(c);
  Circle4(c);
  matrix.show(); // This sends the updated pixel colors to the hardware.

  delay(200);

//  matrix.drawPixel(7,0, c); // x, y, color
//  matrix.drawPixel(7,1, c); // x, y, color
//  matrix.drawPixel(7,2, c); // x, y, color
//  matrix.drawPixel(7,3, c); // x, y, color
//  matrix.drawPixel(7,4, c); // x, y, color
//  matrix.drawPixel(7,5, c); // x, y, color
//  matrix.drawPixel(7,6, c); // x, y, color
//  matrix.drawPixel(7,7, c); // x, y, color
//
//  matrix.show();
}

void Circle1(uint32_t c) {
  matrix.drawPixel(3, 3, c); // x, y, color
  matrix.drawPixel(4, 4, c); // x, y, color
  matrix.drawPixel(3, 4, c); // x, y, color
  matrix.drawPixel(4, 3, c); // x, y, color
}

void Circle2(uint32_t c) {
  matrix.drawLine(3, 2, 4, 2, c); // x0, y0, x1, y1, color
  matrix.drawLine(3, 5, 4, 5, c); // x0, y0, x1, y1, color
  matrix.drawLine(5, 3, 5, 4, c); // x0, y0, x1, y1, color
  matrix.drawLine(2, 3, 2, 4, c); // x0, y0, x1, y1, color
}

void Circle3(uint32_t c) {
  matrix.drawLine(2, 1, 5, 1, c); // x0, y0, x1, y1, color
  matrix.drawLine(2, 6, 5, 6, c); // x0, y0, x1, y1, color
  matrix.drawLine(6, 2, 6, 5, c); // x0, y0, x1, y1, color
  matrix.drawLine(1, 2, 1, 5, c); // x0, y0, x1, y1, color

  //  matrix.drawPixel(2, 2, c); // x, y, color
  //  matrix.drawPixel(5, 5, c); // x, y, color
  //  matrix.drawPixel(5, 2, c); // x, y, color
  //  matrix.drawPixel(2, 5, c); // x, y, color
}

void Circle4(uint32_t c) {

  // Reihe oben
  matrix.drawPixel(0,1, c); // x, y, color
  matrix.drawPixel(0,2, c); // x, y, color
  matrix.drawPixel(0,3, c); // x, y, color
  matrix.drawPixel(0,4, c); // x, y, color
  matrix.drawPixel(0,5, c); // x, y, color
  matrix.drawPixel(0,6, c); // x, y, color
  
  // Reihe links
  matrix.drawPixel(6,0, c); // x, y, color
  matrix.drawPixel(5,0, c); // x, y, color
  matrix.drawPixel(4,0, c); // x, y, color
  matrix.drawPixel(3,0, c); // x, y, color
  matrix.drawPixel(2,0, c); // x, y, color
  matrix.drawPixel(1,0, c); // x, y, color
  
  // Reihe unten
  matrix.drawPixel(6,7, c); // x, y, color
  matrix.drawPixel(5,7, c); // x, y, color
  matrix.drawPixel(4,7, c); // x, y, color
  matrix.drawPixel(3,7, c); // x, y, color
  matrix.drawPixel(2,7, c); // x, y, color
  matrix.drawPixel(1,7, c); // x, y, color

  // Reihe unten
  matrix.drawPixel(7,1, c); // x, y, color
  matrix.drawPixel(7,2, c); // x, y, color
  matrix.drawPixel(7,3, c); // x, y, color
  matrix.drawPixel(7,4, c); // x, y, color
  matrix.drawPixel(7,5, c); // x, y, color
  matrix.drawPixel(7,6, c); // x, y, color


  //  matrix.drawPixel(1, 1, c); // x, y, color
  //  matrix.drawPixel(6, 6, c); // x, y, color
  //  matrix.drawPixel(6, 1, c); // x, y, color
  //  matrix.drawPixel(1, 6, c); // x, y, color
}


