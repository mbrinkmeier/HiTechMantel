#include <Wire.h>
#include <HiTechMantel.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_NeoPixel.h>

#define screenSerial Serial1
#define debugSerial Serial

#define PULSE_PIN A0
#define PULSE_INTERVAL 500
#define PULSE_AVG_COUNT 10

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

// Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1,8,NEO_GRBW + NEO_KHZ800);

byte readByteFromScreen() {
  byte b = screenSerial.available() ? screenSerial.read() : 0;
  delay(1);
  return b;
}

void readBytesFromScreen(int len, byte buf[]) {
  for (int i = 0; i < len; i++) {
    buf[i] = readByteFromScreen();
  }
}

void sendByte(byte id, byte data) {
  Wire.beginTransmission(id);
  Wire.write(data);
  Wire.endTransmission();
}

void sendArray(byte id, byte data[], int length) {
  Wire.beginTransmission(id);
  Wire.write(data,length);
  Wire.endTransmission();
  delay(100);
}


/**
 * Setup as I2C master
 */
void setup() {
  debugSerial.begin(9600);
  screenSerial.begin(9600);
  
  // pixel.begin();
  // Do a selftest
  pixel.setPixelColor(0,255,0,0);
  pixel.show();
  delay(500);
  pixel.setPixelColor(0,0,255,0);
  pixel.show();
  delay(500);
  pixel.setPixelColor(0,0,0,255);
  pixel.show();
  delay(500);
  pixel.setPixelColor(0,0,0,0);
  pixel.show();
  delay(500);
  
  // Initialize I2C as master
  Wire.begin();

  // Initialize serial connections
  debugSerial.println("Init done");
  debugSerial.println("Listen as master");
}


/**
 * Recevie messages fromthe screen
 */
void loop() {
  bool flag = false;
  byte data[255];
  
   // Check if data was received from screen
  if (screenSerial.available() ) {
    byte start = readByteFromScreen();

    // If it was a START_BYTE start processing
    if ( start == START_BYTE ) {
      
      // Start reading TARGET_ID, CMD_ID and DLEN
      byte id = readByteFromScreen();
      byte cmd = readByteFromScreen();
      byte dlen = readByteFromScreen();

      readBytesFromScreen(dlen,data);
      
      // debug Output
      debugSerial.print("id: ");
      debugSerial.print(id);
      debugSerial.print(" cmd: ");
      debugSerial.print(cmd);
      debugSerial.print(" dlen: ");
      debugSerial.print(dlen);
      mantel.debugData(data,dlen);
      debugSerial.println();


      if ( id == 0 ) {
        // If its for the master, do it yourself
        debugSerial.println("Processing commd myself");
      } else {
        // Otherwise send cmd dlen data to id
           debugSerial.println("Relaying data:");
           Wire.beginTransmission(id);
           Wire.write(cmd);
           Wire.write(dlen);
           debugSerial.print(cmd);
           debugSerial.print(" ");
           debugSerial.print(dlen);
           for (int i = 0; i < dlen; i++) {
             Wire.write(data[i]);             
             debugSerial.print(" ");
             debugSerial.print(data[i]);
           }
           debugSerial.println();
           Wire.endTransmission();
      }
    }
  }
  delay(2);
}
