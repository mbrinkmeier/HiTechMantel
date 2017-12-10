/**
 * This sketch is the programm for the thre RGB controlling Floras.
 * 
 * It receives the red, green and blue component to be displayed.
 */
#include <Wire.h>
#include <HiTechMantel.h>

#define debugSerial Serial

// The id of the flora
// int id = ID_BACK;
int id = ID_BELT;
// int id = ID_ARM;

bool debugging = true;

int red = 0;
int green = 0;
int blue = 0;

HiTechMantel mantel = HiTechMantel();

// Floras own pixel
Adafruit_NeoPixel pixel = mantel.pixel;                                  

// the external pixel
Adafruit_NeoPixel pixel2 = Adafruit_NeoPixel(1,10,NEO_GRBW + NEO_KHZ800);


/**
 * Set the Flora up.
 */
void setup() {
  // Start the serial connection for debugging.
  debugSerial.begin(9600);

  // Initialize the two pixel
  pixel.begin();
  pixel2.begin();

  // Run the selftest
  selftest();

  // Start the i2c conection
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  DBG_PRINT(F("Listening on I2C with ID "));
  DBG_PRINTLN(id);
}


/**
 * The main loop does nothing.
 */
void loop() {
  delay(100);
}


/* 
 * Handle a received message
 */
void receiveEvent(int numBytes) {
  byte cmd;   // The command id
  byte dlen;  // The data length
  byte data[255];   // The data bytes

  cmd = mantel.readFromWire();
  dlen = mantel.readFromWire();

  mantel.readData(dlen,data);
  mantel.emptyWire();

  DBG_PRINT(F("Received cmd: "));
  DBG_PRINT(cmd);
  DBG_PRINT(F(" dlen: "));
  DBG_PRINT(dlen);
  mantel.debugData(data,dlen);  
  
  switch (cmd) {
    case CMD_RGB_RESET:
      pixel2.setPixelColor(0,0,0,0);
      pixel.setPixelColor(0,0,0,0);
      pixel2.show();
      pixel.show();
      red = 0;
      green = 0;
      blue = 0;
      break;
    case CMD_SELFTEST:
      selftest();
      break;
    case CMD_RGB_SET:
      // Set the color
      red = data[0];
      green = data[1];
      blue = data[2];
      // setColor();
      break;
   case CMD_RGB_RED:
     red = data[0];
     setColor();
     break;
   case CMD_RGB_GREEN:
     green = data[0];
     setColor();
     break;
   case CMD_RGB_BLUE:
     blue = data[0];
     setColor();
     break;
   case CMD_PING:
      Wire.onRequest(waitForPing);
      break;
  }
}




/**
 * Wait for the ping request.
 */
void waitForPing() {
  Wire.write("ping");
  Wire.onRequest(NULL);
}




/** 
 * Selftest
 * 
 * Cycle through all colors.
 */
void selftest() {

  pixel2.setPixelColor(0,255,0,0);
  pixel2.show();
  pixel.setPixelColor(0,255,0,0);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,255,0);
  pixel2.show();
  pixel.setPixelColor(0,0,255,0);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,0,255);
  pixel2.show();
  pixel.setPixelColor(0,0,0,255);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,255,255,0);
  pixel2.show();
  pixel.setPixelColor(0,255,255,0);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,255,0,255);
  pixel2.show();
  pixel.setPixelColor(0,255,0,255);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,255,255);
  pixel2.show();
  pixel.setPixelColor(0,0,255,255);
  pixel.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,0,0);
  pixel2.show();
  pixel.setPixelColor(0,0,0,0);
  pixel.show();
  delay(500);
}



/**
 * Set the color of both pixels
 */
void setColor() {
  pixel2.setPixelColor(0,red,green,blue);
  pixel.setPixelColor(0,red,green,blue);
  pixel.show();
  pixel2.show();
}

