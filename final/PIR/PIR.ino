/**
 * This sketch schecks, wether a signal was received from a
 * digital PIR sensor during the last interval of time.
 * 
 * The interval can be set using I2C:
 * 1 <intHi> <intLo>
 * 
 * If the interval is set to 0, it is checked if some signal
 * was detected since the last request.
 */
#include <Wire.h>
#include <HiTechMantel.h>

#define PIR_PIN 10

// int id = ID_PIR_FRONT;
int id = ID_PIR_BACK;

unsigned long interval;
unsigned long lastTick;
byte detected;

HiTechMantel mantel = HiTechMantel();

Adafruit_NeoPixel pixel = mantel.pixel;

void setup() {
  // Set the pin mode for the PIR Pin
  pinMode(PIR_PIN, INPUT);
  interval = 300000; // Monitoring interval of 5 min
  pixel.begin();
  pixel.setPixelColor(0,0,0,0);
  pixel.show();
    
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event

  selftest();
  
  Serial.begin(9600);

  Serial.print("Listening as ID ");
  Serial.println(id);
  Serial.flush();

}


/*
 * Check the sensor and store a received signal.
 * 
 * interval == 0 means no measurmente
 */
void loop() {
  if ( interval > 0 ) {
    long curTick = millis();
    if ( curTick - lastTick >= interval ) {
      // If Interval is over
      detected = 0;
      lastTick = curTick;
      pixel.setPixelColor(0,0,0,0);
      pixel.show();
    }
  }

  int value = digitalRead(PIR_PIN);
  digitalWrite(7,value);
  
  detected = detected || ( value == HIGH);
  if ( detected ) {
      pixel.setPixelColor(0,255,0,0);
      pixel.show();    
  }
  delay(10);
}

/* 
 * Send the detected signal and reset. 
 */
void requestEvent() {
  Wire.write(detected);
  // Reset
  detected = 0;
  lastTick = millis();
}


/*
 * Set the interval, if a message is received.
 */
void receiveEvent() {
  byte cmd;   // The command id
  byte dlen;  // The data length
  byte data[255];   // The data bytes

  cmd = mantel.readFromWire();
  dlen = mantel.readFromWire();
  mantel.readData(dlen,data);

  Serial.print("Received cmd: ");
  Serial.print(cmd);
  Serial.print(" dlen: ");
  Serial.println(dlen);
  mantel.debugData(data,dlen);
  Serial.flush();


  switch (cmd) {
    case CMD_PIR_RESET:
      interval = 300000;
      detected = 0;
      break;
    case CMD_PIR_SET:
      // read the first two bytes as intervall
      byte lo = dlen > 0 ? data[0] : 0;
      byte hi = dlen > 1 ? data[1] : 0;
      int sec = 256 * hi + lo;
      interval = 1000l*sec;
      Serial.print("Interval set to ");
      Serial.println(interval);
      Serial.flush();
      break;
  }
  mantel.emptyWire();
}

/**
 * A simple selftest
 */
void selftest() {
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
}

