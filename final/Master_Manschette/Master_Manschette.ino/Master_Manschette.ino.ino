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
// #include <I2C.h>
#include <Wire.h>
#include <HiTechMantel.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_NeoPixel.h>

#define PULSE_PIN A10
#define PULSE_INTERVAL 500
#define PULSE_AVG_COUNT 10

#define SLEEPTIME 1000 // 1 min till sleep
#define DEEPSLEEPTIME 10000 // 300 sec till deep sleep

unsigned long lastActivity;   // The time of the last acitivty
bool sleeping = false;        // A flag indicating the sleep mode
bool sleepingDeep = false;    // A flag indicting the deep sleep mode

bool alarm = false;           // A flag indicating wether an alarm should be given if motion is detected by the back sensor
byte motionThreshold = 1;     // Number of subsequent intervals of motionafter which the coat wakes up or starts an alarm

bool measuringPulse = false;  // Various values for measuring the pulse
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
  // I2c.begin();
  // I2c.timeOut(1000);
  Wire.begin();

  // Initialize serial connections
  debugSerial.println(F("Init done"));
  debugSerial.println(F("Listen as master"));

  mantel.writeToScreen("page 0");
}


/**
 * Recevie messages from the screen
 */
void loop() {
  bool flag = false;
  byte data[256];

  // Check for alarm
  if (alarm && getBackPirActivity() ) {
     startAlarm();
  }
  
  // Measure pulse
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
    
    byte start = mantel.readByteFromScreen();
    // debugSerial.print("Received byte ");
    // debugSerial.println(start);
  
    // If it was a START_BYTE start processing
    if ( start == START_BYTE ) {
      
      // Start reading TARGET_ID, CMD_ID and DLEN
      byte id = mantel.readByteFromScreen();
      byte cmd = mantel.readByteFromScreen();
      byte dlen = mantel.readByteFromScreen();

      mantel.readBytesFromScreen(data,dlen);

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
          default:
            wakeUp();
            break;
        }
      } else {
        // Otherwise send cmd dlen data to id

        // I2c.write(id,cmd,data,dlen+1);
        mantel.writeBytesToSlave(id,cmd,data,dlen+1);
                
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
  
  if ( (time > DEEPSLEEPTIME) && sleeping && (!sleepingDeep) ) {
    goToSleep(true);
  } else if ((time > SLEEPTIME) && !sleeping ) {
    goToSleep(false);
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
  if ( sleepingDeep ) {
    // From deep sleep awaken with more action
    char text[24] = "Hallo! Probier mich aus!";
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_RED,0);
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_GREEN,255);
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_BLUE,0);
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_COLOR,0);
    // mantel.writeBytesToSlave(ID_MATRIX,CMD_MATRIX_TEXT,text,24);
  } else {
    // If the sleep is nt to deep, just wake up
    mantel.writeToScreen(F("page 0"));
  }
  sleeping = false;
  sleepingDeep = false;
  lastActivity = millis();
}


/**
 * Go to sleep
 * 
 * Reset all Floras
 */
void goToSleep(bool deep) {
  // check Pir activity before going to sleep
  if ( getPirActivity() ) return;

  sleeping = true;
  measuringPulse = false;
  // Send all Floras the reset command
  // I2c.write(ID_BACK,0);
  // I2c.write(ID_ARM,0);
  // I2c.write(ID_BELT,0);
  // I2c.write(ID_STRIP,0);
  // I2c.write(ID_MATRIX,0);
  // I2c.write(ID_MP3,0);
  // I2c.write(ID_MOTOR,0);
  mantel.writeByteToSlave(ID_BACK,0);
  mantel.writeByteToSlave(ID_ARM,0);
  mantel.writeByteToSlave(ID_BELT,0);
  mantel.writeByteToSlave(ID_STRIP,0);
  mantel.writeByteToSlave(ID_MATRIX,0);
  mantel.writeByteToSlave(ID_MP3,0);
  mantel.writeByteToSlave(ID_MOTOR,0);  // Send sleep signal to screen
  if ( deep ) {
    debugSerial.println(F("Going to deep sleep!"));
    sleepingDeep = true;
    mantel.writeToScreen(F("page 14"));
  } else {
    debugSerial.println(F("Going to sleep!"));
    mantel.writeToScreen(F("page 13"));
  }
}

/**
 * Check both PIRS for activity in the last perios of time
 */
bool getPirActivity() {
  Serial.println(F("Checking pir activity"));
  
  bool active = false;
  byte data[1];
  
  // Get Front PIR
  // I2c.read(ID_PIR_FRONT,1,1);

  byte b = mantel.readByteFromSlave(ID_PIR_FRONT);
  // byte b = I2c.receive();
  // while (I2c.available() > 0) I2c.receive();
  active = active || ( b >= motionThreshold );

  if ( b != 0 ) debugSerial.println(F("Front PIR activity detected"));

  // Get Back PIR
  // I2c.read(ID_PIR_BACK,1,1);

  b = mantel.readByteFromSlave(ID_PIR_BACK);
  // while (I2c.available() > 0) I2c.receive();
  active = active || ( b >= motionThreshold );

  Serial.print(F("Received "));
  Serial.println(b);

  if (b != 0) debugSerial.println(F("Back PIR activity detected"));

  return active;
}


/**
 * Check both PIRS for activity in the last perios of time
 */
bool getBackPirActivity() {
  bool active = false;
  byte data[1];
  byte b;
  
  // Get Back PIR
  // I2c.read(ID_PIR_BACK,1,data);
  
  b = mantel.readByteFromSlave(ID_PIR_BACK);
  active = active || ( b >= motionThreshold );

  if (b != 0) debugSerial.println(F("Back PIR activity detected"));

  return active;
}


/**
 * Welcome a user
 */
void runWelcome() {
}


/**
 * Alarm!!
 */
void startAlarm() {
}


