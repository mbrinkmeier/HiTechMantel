/**
 * This sketch schecks, wether a signal was received from a
 * digital PIR sensor during the last interval of time.
 * 
 * The interval can be set using I2C:
 * 1 <intHi> <intLo>
 * 
 * If the interval is set to 0, it is checked if some signal
 * was detected since the last request.
 *   
 *  Copyright (c) 2017 Michael Brinkmeier (michael.brinkmeier@uni-osnabrueck.de)
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <Wire.h>
#include <HiTechMantel.h>

#define debugSerial Serial

#define PIR_PIN 10



// int id = ID_PIR_FRONT;
// bool inverseLogic = false;
// #define MOTION_MIN_LENGTH 1000

int id = ID_PIR_BACK;       // The back sensor requires inverse logic
bool inverseLogic = true;
#define MOTION_MIN_LENGTH 250


unsigned long interval;    // The interval length
unsigned long lastMove;    // The time of the last detected motion

bool motionDetected = false;      // A flag indicating wether motion is currently going on
unsigned long motionDuration = 0; // The duration of the last detected motion
byte motions = 0;                 // The number of subsequent intervals in which a motion was detected
byte detected;

HiTechMantel mantel = HiTechMantel();

Adafruit_NeoPixel pixel = mantel.pixel;

void setup() {
  // Set the pin mode for the PIR Pin
  pinMode(PIR_PIN, INPUT);
  
  interval = 1000; // default monitoring interval of 0.5 sec
  
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

  delay(3000);
}


/*
 * Check the sensor and store a received signal.
 * 
 * interval == 0 means no measurmente
 */
void loop() {
  if ( interval > 0 ) {
  
    int value = digitalRead(PIR_PIN);
    if ( inverseLogic ) {
      if ( value == HIGH ) {
        value = LOW;
      } else {
        value = HIGH;
      }
    }

    
    digitalWrite(7,value);

    // Motion starts
    if ( value == HIGH ) {
      if ( motionDetected == false ) {
        pixel.setPixelColor(0,255,0,0);
        pixel.show();    
        motionDetected = true;
        lastMove = millis();
        debugSerial.println(F("Motion starts"));
      } else {
        motionDuration = millis() - lastMove;
        if (motionDuration > MOTION_MIN_LENGTH) {
           motions += motionDuration/MOTION_MIN_LENGTH;
           lastMove = millis();     
           debugSerial.print(F("Motion going on "));
           debugSerial.println(motions);
        }
      }
      
    }
    
    // Motion ends
    if ( ( value == LOW ) && ( motionDetected == true ) ) {
      motionDuration = millis() - lastMove;
      if (motionDuration > MOTION_MIN_LENGTH) {
        motions += motionDuration/MOTION_MIN_LENGTH;  
      }
      motionDetected = false;
      debugSerial.println(F("Motion ends"));
      debugSerial.print(F("Number of detected motions : "));
      debugSerial.println(motions);
    }
  
    unsigned long curTime = millis();

    if ( (curTime - lastMove) > interval ) {
      motions = 0;
      pixel.setPixelColor(0,0,0,0);
      pixel.show();        
    }
  }  
  delay(10);
}

/* 
 * Send the detected signal and reset. 
 */
void requestEvent() {
  Serial.println(F("Request received"));
  Serial.print(F("Send answer: "));
  Wire.write(motions);
  Serial.println(motions);
  /*
  unsigned long curTime = millis();
  if ( (curTime - lastMove) <= interval ) {
     Wire.write(motions);
     Serial.println(motions);
  } else {
    Wire.write(0);
    Serial.println(0);
  }*/
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
  delay(500);
}

