#include <Wire.h>
#include <HiTechMantel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#define screenSerial Serial1
#define debugSerial Serial

#define PULSE_PIN A0
#define PULSE_INTERVAL 500
#define PULSE_AVG_COUNT 10


Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1,8,NEO_GRBW + NEO_KHZ800);

byte readByteFromScreen() {
  byte b = screenSerial.available() ? screenSerial.read() : 0;
  delay(1);
  return b;
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



void setup() {
  debugSerial.begin(9600);
  screenSerial.begin(9600);
  
  pixel.begin();
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


void loop() {
  bool flag = false;

  if (screenSerial.available() ) {
    digitalWrite(7,HIGH);
    byte start = readByteFromScreen();
    if ( start == START_BYTE ) {
      // Start reading a command
      byte id = readByteFromScreen();
      byte cmd = readByteFromScreen();
      byte data = readByteFromScreen();

      debugSerial.print("id: ");
      debugSerial.print(id);
      debugSerial.print(" cmd: ");
      debugSerial.print(cmd);
      debugSerial.print(" data: ");
      debugSerial.println(data);
      // Now treat the commands, depending on their data length

      switch (id) {
        case ID_BACK:
        case ID_ARM:
        case ID_BELT:
           if ( cmd == CMD_RGB_SET ) {
             // Extract the colors from the data byte
             byte color[4];
             color[0] = CMD_RGB_SET;
             color[1] = 255 * (data % 2);
             data = data / 2;
             color[2] = 255 * (data % 2);
             data = data / 2;
             color[3] = 255 * (data % 2);
             // Set Pixel
             debugSerial.print("RGB set id ");
             debugSerial.print(id);
             debugSerial.print(" ");
             debugSerial.print(color[1]);
             debugSerial.print(" ");
             debugSerial.print(color[2]);
             debugSerial.print(" ");
             debugSerial.println(color[3]);
             pixel.setPixelColor(0,color[1],color[2],color[3]);
             pixel.show();
             // Send values
             sendArray(id,color,4);
           } else {
             sendByte(id,cmd); 
           }
           break;
        default:
           // Simply send the next <data> bytes to the corresponding id
           debugSerial.println("Relaying data:");
           Wire.beginTransmission(id);
           Wire.write(cmd);
           Wire.write(data);
           debugSerial.print(cmd);
           debugSerial.print(" ");
           debugSerial.print(data);
           byte count = data;
           while ((count > 0) && (screenSerial.available())) {
             byte d = readByteFromScreen();
             debugSerial.print(" ");
             debugSerial.print(d);
             Wire.write(d); 
           }
           debugSerial.println();
           Wire.endTransmission();
      }
    }
  } else {
    digitalWrite(7,LOW);
  }
  delay(2);
}
