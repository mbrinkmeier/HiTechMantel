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

#define PIR 10

int id = PIR_FRONT_ID;

int interval;
long lastTick;
char detected;

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
  char reg;
  char intHi;
  char intLo;

  reg = Wire.read();

  switch (reg) {
    case RESET_REG:
      interval = 0;
      detected = 0;
      break;
    case SELFTEST_REG:
      break;
    case PIR_SET_REG:
      intHi = Wire.available() ? Wire.read() : 0;
      intLo = Wire.available() ? Wire.read() : 0;
      interval = intHi * 256 + intLo;
      lastTick = millis();
      break;
  }
}


void setup() {
  // Set the pin mode for the PIR Pin
  pinMode(PIR, INPUT);
  
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
}

/*
 * Check the sensor and store a received signal.
 */
void loop() {
  if ( interval > 0 ) {
    long curTick = millis();
    if ( curTick - lastTick >= interval ) {
      detected = 0;
      lastTick = curTick;
    }
  }

  detected = detected | (char) digitalRead(PIR);
  delay(100);
}
