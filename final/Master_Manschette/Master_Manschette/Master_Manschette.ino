/** 
 *  This sketch is the programm for I2C Master of the HiTechCoat Project.
 *  It communicates with the Nextion TouchScreen and relays commands to the other
 *  Floras and the LilyPad Mp3.
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
 #include <I2C.h>
#include <HiTechMantel.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_NeoPixel.h>

#define screenSerial Serial1
#define debugSerial Serial

#define PULSE_PIN A10
#define PULSE_INTERVAL 500
#define PULSE_AVG_COUNT 10

#define SLEEPTIME 30000 // 30 sec till sleep

unsigned long lastActivity;   // The time of the last acitivty
bool sleeping = false;
bool sleepingDeep = false;

bool measuringPulse = false;
int pulseMin;
int pulseMax;
int pulseMid;
int pulseTick;
int pulseDur;
int pulseStart;
int pulses[10];
int pulsePos;
int pulseSum;
bool pulse;


HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;


byte readByteFromScreen() {
  byte b = screenSerial.available() ? screenSerial.read() : 0;
  delay(1);
  return b;
}

void readBytesFromScreen(int len, byte buf[]) {
  buf[0] = len;
  for (int i = 1; i <= len; i++) {
    buf[i] = readByteFromScreen();
  }
}


/**
 * Send a command to screen
 */
void sendToScreen(String cmd) {
  screenSerial.print(cmd);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);  
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
  I2c.begin();
  I2c.timeOut(1000);

  // Initialize serial connections
  debugSerial.println(F("Init done"));
  debugSerial.println(F("Listen as master"));

  sendToScreen("page 0");
}


/**
 * Recevie messages from the screen
 */
void loop() {
  bool flag = false;
  byte data[256];

  // Measur pulse
  if (measuringPulse) {
    measurePulse();  
  }

  
  // If sleeping and pir detected movement, wake up.
  if ( sleeping && getPirActivity() ) {
    wakeUp();
    runWelcome();
    lastActivity = millis();
  }

   // Check if data was received from screen
  while (screenSerial.available() ) {
    if ( sleeping ) wakeUp(); // wake up
    lastActivity = millis();
    
    byte start = readByteFromScreen();
    // debugSerial.print("Received byte ");
    // debugSerial.println(start);
  
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
      mantel.debugData(data,dlen);
      debugSerial.println();

      mantel.emptyWire();

      if ( id == 0 ) {
        // If its for the master, do it yourself
        debugSerial.println(F("Processing command myself"));
        switch (cmd) {
          case CMD_PULSE_START:
            debugSerial.println(F("Starting pulse measurement"));
            pulseMin = 255;
            pulseMid = 127;
            pulseMax = 0;
            pulse = false;
            pulseTick = 0;
            pulsePos = 0;
            pulseSum = 0;
            measuringPulse = true;
            break;
          case CMD_PULSE_STOP:
            measuringPulse = false;
            debugSerial.println(F("Stopping pulse measurement"));
            break;
          case CMD_PULSE_SHOW:
            break;
        }
      } else {
        // Otherwise send cmd dlen data to id

        I2c.write(id,cmd,data,dlen+1);
                
        debugSerial.println("Relayed data:");
        debugSerial.print(cmd);
        debugSerial.print(" ");
        debugSerial.print(dlen);
        for (int i = 0; i < dlen; i++) {
          debugSerial.print(" ");
          debugSerial.print(data[i]);
        }
        debugSerial.println();
        debugSerial.flush();
      }
    }
  }

  unsigned long time = millis() - lastActivity;
  
  if ( time > 2*SLEEPTIME ) {
    if ( sleeping ) {
      if ( !sleepingDeep ) {
        sleepingDeep = true;
        sendToScreen(F("page 14"));
        
      }
    } else {
      goToSleep();
    }
  }
  delay(10);
}


/**
 * Measure the pulse and display it on screen.
 */
void measurePulse() {
  pulseTick++;
  int pulseVal;
  
  int pulseRaw = analogRead(PULSE_PIN);
  pulsePos = (pulsePos+1)%10;
  pulseSum = pulseSum - pulses[pulsePos] + pulseRaw;
  pulses[pulsePos] = pulseRaw;
  pulseVal = pulseSum/10;
  pulseRaw = pulseVal;
  
  if ( pulseRaw < pulseMin ) {
    pulseMin = pulseRaw;
  } else if ( pulseRaw > 11 * pulseMin / 10) {
    pulseMin = 11 * pulseMin / 10;
  }
  if ( pulseRaw > pulseMax ) {
    pulseMax = pulseRaw;
  } else if ( pulseRaw < 9 * pulseMax / 10) {
    pulseMax = 9 * pulseMax / 10;
  }
  pulseMid = (pulseMax - pulseMin)/2;

  if ( (pulseRaw > pulseMid) && (pulse == false) ) {
    pulse = true;
    long now = millis();
    pulseDur = now - pulseStart;
    pulseTick = 0;
    pulseStart = now;
  } else if ( (pulseRaw < pulseMid) && (pulse == true) ) {
    pulse = false;
  }

  // scale it to screen
  int pulseValue = map(pulseRaw,pulseMin,pulseMax,0,255);

  int pulseFreq = 1000 / pulseDur;
  
  screenSerial.print("pulseText.txt=\"");
  screenSerial.print(pulseRaw);
  screenSerial.print("\"");
  // screenSerial.print(pulseFreq);
  // screenSerial.print(" bpm\"");
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);  
  screenSerial.print("add 1,0,");
  screenSerial.print(pulseVal);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
}


/**
 * Waking up
 * 
 * Start welcoming program
 */
void wakeUp() {
  debugSerial.println(F("Waking Up!"));
  sleeping = false;
  sleepingDeep = false;
  sendToScreen(F("page 0"));
}

/**
 * Go to sleep
 * 
 * Reset all Floras
 */
void goToSleep() {
  sleeping = true;
  measuringPulse = false;
  debugSerial.println(F("Going to sleep!"));
  // Send all Floras the reset command
  I2c.write(ID_BACK,0);
  I2c.write(ID_ARM,0);
  I2c.write(ID_BELT,0);
  I2c.write(ID_STRIP,0);
  I2c.write(ID_MATRIX,0);
  I2c.write(ID_MP3,0);
  I2c.write(ID_MOTOR,0);
  // Send sleep signal to screen
  sendToScreen(F("page 13"));
}

/**
 * Check both PIRS for activity in the last perios of time
 */
bool getPirActivity() {
  bool active = false;
  byte data[1];
  
  // Get Front PIR
  I2c.read(ID_PIR_FRONT,1,data);

  byte b = data[0];
  active = active || ( b != 0 );

  if ( b != 0 ) debugSerial.println(F("Front PIR activity detected"));

  // Get Back PIR
  I2c.read(ID_PIR_BACK,1,data);

  b = data[0];
  active = active || ( b != 0 );

  if (b != 0) debugSerial.println(F("Back PIR activity detected"));

  return active;
}

/**
 * Welcome a user
 */
void runWelcome() {
}

