/**
 * This sketch is the programm for the thre RGB controlling Floras.
 * 
 * It receives the red, green and blue component to be displayed.
 */
#include <Wire.h>
#include <HiTechMantel.h>



// The id of the flora

//int id = ID_BACK;
// int id = ID_BELT;
int id = ID_ARM;

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

Adafruit_NeoPixel pixel2 = Adafruit_NeoPixel(1,10,NEO_GRBW + NEO_KHZ800);

/**
 * Set the Flora up.
 */
void setup() {
  // Start the serial connection for debugging.
  Serial.begin(9600);
  pixel2.begin();
  

  // Run the selftest

  // Start the i2c conection
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  Serial.print("Listening on I2C with ID ");
  Serial.println(id);

    selftest();

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

  Serial.print("Received cmd: ");
  Serial.print(cmd);
  Serial.print(" dlen: ");
  Serial.println(dlen);

  mantel.readData(dlen,data);
  
  switch (cmd) {
    case CMD_RGB_RESET:
      pixel2.setPixelColor(0,0,0,0);
      pixel.setPixelColor(0,0,0,0);
      pixel2.show();
      pixel.show();
      break;
    case CMD_SELFTEST:
      selftest();
      break;
    case CMD_RGB_SET:
      // Set the color
      pixel2.setPixelColor(0,data[0],data[1],data[2]);
      pixel.setPixelColor(0,data[0],data[1],data[2]);
      Serial.println(data[0]);
      Serial.println(data[1]);
      Serial.println(data[2]);
      pixel.show();
      pixel2.show();
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

  delay(500);
  
  pixel2.setPixelColor(0,0,255,0);
  pixel2.show();

  delay(500);
  
  pixel2.setPixelColor(0,0,0,255);
  pixel2.show();

  delay(500);
  
  pixel2.setPixelColor(0,255,255,0);
  pixel2.show();
  delay(500);
  
  pixel2.setPixelColor(0,255,0,255);
  pixel2.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,255,255);
  pixel2.show();
  delay(500);
  
  pixel2.setPixelColor(0,0,0,0);
  pixel2.show();
  delay(500);
}

