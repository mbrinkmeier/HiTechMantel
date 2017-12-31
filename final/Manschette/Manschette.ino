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
#include <Wire.h>
#include <HiTechMantel.h>

#define PULSE_PIN A10
#define PULSE_AVG_COUNT 10

#define SLEEPTIME 300000 // 1 min till sleep
#define DEEPSLEEPTIME 600000 // 300 sec till deep sleep

byte data[256];               // An array for buffering received data

unsigned long lastActivity;   // The time of the last acitivty
bool sleeping = false;        // A flag indicating the sleep mode
bool sleepingDeep = false;    // A flag indicting the deep sleep mode

bool alarm = false;           // A flag indicating wether an alarm should be given if motion is detected by the back sensor
byte motionThreshold = 1;     // Number of subsequent intervals of motionafter which the coat wakes up or starts an alarm

bool measuringPulse = false;  // Various values for measuring the pulse

int pulseMin;                  // The minimal sample
int pulseMax;                  // The maximal sample
int pulseMid;                  // The medium value
unsigned long pulseTick;       // pulse tick
unsigned long pulseLAstSample; // pulse tick
unsigned long pulseLastBeat;   // pulse tick
int pulseRate[10];             // Array to save the pulse rates
int pulseRateIdx;              // Index for the next rate
int pulseRateSum;                  // The sum of all current rates
int pulseRateAvg;
bool pulseFirstBeat;
bool pulseSecondBeat;
bool pulse;
int pulseIBI;
int pulseBPM;
int pulseOldBPM;
int pulseSample[10];
int pulseSampleIdx;
int pulseSampleSum;
int pulseMaxView;
int pulseMinView;
bool pulseShowing;



HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;


/**
 * Setup as I2C master
 */
void setup() {
  // Set the mode for the pulse pin
  pinMode(PULSE_PIN,INPUT);

  // Start serial connections
  debugSerial.begin(9600);
  screenSerial.begin(9600);
  
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
  debugSerial.println(F("Init done"));
  debugSerial.println(F("Listen as master"));

  mantel.writeToScreen(F("page 0"));

  pulseShowing = false;
}


/**
 * Recevie messages from the screen
 */
void loop() {
  byte start, id, cmd, dlen;
  
  // Check for alarm
  if (alarm && getBackPirActivity() ) {
     startAlarm();
  }
  
  
  // If sleeping and pir detected movement, wake up.
  if ( sleeping && getPirActivity() ) {
    wakeUp(true);
    lastActivity = millis();
  }


  // Measure pulse
  if (measuringPulse) {
    measurePulse();  
  }

  start = 0;
  
  // Check if data was received from screen
  if (screenSerial.available() ) {
    if ( sleeping ) wakeUp(false); // wake up
    lastActivity = millis();
    
    start = mantel.readByteFromScreen();
    debugSerial.println(start);
  
    // If it was a START_BYTE start processing
    if ( start == START_BYTE ) {
      
      // Start reading TARGET_ID, CMD_ID and DLEN
      id = mantel.readByteFromScreen();
      cmd = mantel.readByteFromScreen();
      dlen = mantel.readByteFromScreen();

      mantel.readBytesFromScreen(data,dlen);

      // debug Output
      debugSerial.println(F("Received command from screen"));

      mantel.emptyWire();
    }
  }


  if ( (start != START_BYTE) && debugSerial.available() ) {
    if ( sleeping ) wakeUp(false); // wake up
    lastActivity = millis();
    
    start = debugSerial.parseInt();
  
    // If it was a START_BYTE start processing
    if ( start == START_BYTE ) {
      
      // Start reading TARGET_ID, CMD_ID and DLEN
      id = debugSerial.parseInt();
      cmd = debugSerial.parseInt();
      dlen = debugSerial.parseInt();

      for (int i = 0; i < dlen; i++ ) {
        data[i] = debugSerial.parseInt();
      }

      // debug Output
      debugSerial.println(F("Received command from debug serial"));
    }
  }

  
  if (start == START_BYTE) {
    debugSerial.print("id: ");
    debugSerial.print(id);
    debugSerial.print(" cmd: ");
    debugSerial.print(cmd);
    debugSerial.print(" dlen: ");
    mantel.debugData(data,dlen);
    debugSerial.println();

    if (id == 0) {
        // If its for the master, do it yourself
        debugSerial.println(F("Processing command myself"));
        switch (cmd) {
          case CMD_PULSE_START:
            debugSerial.println(F("Starting pulse measurement"));
            pulseMin = 1023;
            pulseMid = 512;
            pulseMax = 0;
            pulseMinView = 200;
            pulseMaxView = 900;
            pulse = false;
            pulseTick = 0;
            pulseRateSum = 0;
            pulseLastBeat = millis();
            pulseRateIdx = 0;
            pulseFirstBeat = true;
            pulseSecondBeat = false;
            measuringPulse = true;
            pulseIBI = 0;
            pulseBPM = 0;
            pulseOldBPM = 0;
            pulseShowing = true;
              mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_PULSE,pulseBPM);              
            for (int i = 0 ; i < 10; i++) {
              pulseRate[i] = 0;
              pulseSample[i] = 0;
            }
            pulseSampleIdx = 0;
            pulseSampleSum = 0;
            break;
          case CMD_PULSE_STOP:
            measuringPulse = false;
            pulseShowing = false;
            debugSerial.println(F("Stopping pulse measurement"));
              mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_RESET);  
            break;
          case CMD_PULSE_SHOW:
            if ( data[0] == 0 ) {
              pulseShowing = false;
              mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_RESET);  
            } else {
              pulseShowing = true;
              mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_PULSE,pulseBPM);              
            }
            break;
          default:
            wakeUp(false);
            break;
        }
      } else  {
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
  
  // Check time since last activity and go to sleep
  unsigned long time = millis() - lastActivity;

  if ( (time > DEEPSLEEPTIME) && sleeping && (!sleepingDeep) ) {
    goToSleep(true);
  } else if ((time > SLEEPTIME) && !sleeping ) {
    goToSleep(false);
  }
 
  delay(5); 
}


/**
 * Measure the pulse and display it on screen.
 */
void measurePulse() {
  // Read the pulse value
  int pulseRaw = analogRead(PULSE_PIN);
  unsigned long now = millis();
 
  pulseSampleSum = pulseSampleSum - pulseSample[pulseSampleIdx];
  pulseSample[pulseSampleIdx] = pulseRaw;
  pulseSampleSum = pulseSampleSum + pulseSample[pulseSampleIdx];
  pulseSampleIdx = (pulseSampleIdx+1) % 1;
  pulseRaw = pulseSampleSum / 1;
    
  // Calculate the time since last beat
  pulseTick = now - pulseLastBeat;

  // Update the lowest value, if at least 3/5 of the last measure pulse duration passed.
  // if ( ( pulseRaw < pulseMid ) && ( pulseTick > ( pulseIBI / 5 ) * 3 ) ) {
    if ( pulseRaw < pulseMin ) {
      pulseMin = pulseRaw;
    }
  // }

  // Update the maximum value
  if ( ( pulseRaw > pulseMid ) && ( pulseRaw > pulseMax ) ) {
    pulseMax = pulseRaw;
  }

  // If at least 250 ms passed, no pulse was detected yet and at leas 3/5 of the last pulse duration
  // passed, check if the raw signal is above the current threshold.
  if ( pulseTick > 250 ) {
    if ( ( pulseRaw > pulseMid ) && ( pulse == false ) && ( pulseTick > ( pulseIBI/5 )*3 ) ) {
      pulse = true;          // Pulse detected
      pulseIBI = pulseTick;  // Store current length
      pulseLastBeat = now;   // Store the current time

      // If it is the second beat, initialize with the current pulse duration
      if ( pulseSecondBeat ) {
        pulseSecondBeat = false;
        for ( int i = 0; i <= 0; i++ ) {
          pulseRate[i] = pulseIBI;
        }
        pulseRateSum = 10 * pulseIBI;
        pulseRateAvg = pulseIBI;
      }

      // Ignore the first beat
      if ( pulseFirstBeat ) {
        pulseFirstBeat = false;
        pulseSecondBeat = true;
        pulseLastBeat = now;
        return;
      }

      // Store the current IBI in the rate array and compute the sliding average
      pulseRateSum = pulseRateSum - pulseRate[pulseRateIdx];
      pulseRate[pulseRateIdx] = pulseIBI;
      pulseRateSum = pulseRateSum + pulseRate[pulseRateIdx];
      pulseRateIdx = (pulseRateIdx+1) % 10;
      pulseRateAvg = pulseRateSum / 10;

      // Compute the BPM
      pulseBPM = 60000 / pulseRateAvg;
    }
  }

  // If the signal falls beneath the threshold, set the new threshold
  // and intialize a new array
  if ( ( pulseRaw < pulseMid ) && ( pulse == true ) ) {
      pulse = false;
      // store the old values for the view
      pulseMinView = pulseMin;
      pulseMaxView = pulseMax;
      // Reset everythings
      pulseMid = (pulseMax + pulseMin) / 2;
      pulseMin = 1023;
      pulseMax = 0;
  }

  // If no pulse was detected in the last 2.5 seconds, reset everything
  if ( pulseTick > 2500 ) {
    pulseMid = (pulseMax+pulseMin)/2;
    pulseMax = 0;
    pulseMin = 1023;
    pulseLastBeat = now;
    pulseFirstBeat = true;
    pulseSecondBeat = false;
    pulse = false;
    pulseRateSum = 0;
    for ( int i = 0; i <= 0; i++ ) {
      pulseRate[i] = 0;
    }    
  }

  // Scale everything to the screen.
  // The scale varies with the values of the last measured pulse  
  pulseMinView = pulseMid - (pulseMax-pulseMin);
  pulseMaxView = pulseMid + (pulseMax-pulseMin);
  // int pulseValue = map(pulseRaw,pulseMid/2,2*pulseMid,0,255);
  int amp = (pulseMaxView - pulseMinView);
  int center = (pulseMaxView + pulseMinView)/2;
  int pulseValue = map(pulseRaw,center-3*amp/2,center+3*amp/2,0,255);

  screenSerial.print("pulseText.txt=\"");
  screenSerial.print(pulseBPM);
  screenSerial.print(" bpm\"");
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);  
  screenSerial.print("add 1,0,");
  screenSerial.print(pulseValue);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.print("add 1,1,");
  // screenSerial.print(map(pulseRaw,0,min(pulseMaxView*2,1023),0,255));
  // screenSerial.print(pulseFirstBeat*10);
  screenSerial.print(10*pulse);
  // screenSerial.print(map(pulseTick,0,3000,0,255));
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);

  
  if ( (pulseShowing) && ( pulseBPM != pulseOldBPM ) ) {
    pulseOldBPM = pulseBPM;
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_SPEED,pulseBPM);
  }
}


/**
 * Waking up
 * 
 * The flag indicates, wether the welcoming program should be started
 */
void wakeUp(bool loud) {
  debugSerial.println(F("Waking Up!"));
  if ( sleepingDeep && loud) {
    // From deep sleep awaken with more action
    char text[24] = "Hallo! Probier mich aus!";
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_RED,0);
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_GREEN,255);
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_BLUE,0);
    mantel.writeByteToSlave(ID_MATRIX,CMD_MATRIX_COLOR,0);
    // mantel.writeBytesToSlave(ID_MATRIX,CMD_MATRIX_TEXT,text,24);
  } else {
    // If the sleep is not to deep, just wake up
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
  
  bool active = getFrontPirActivity();
  active = active || getBackPirActivity();

  return active;
}

/**
 * Check both PIRS for activity in the last perios of time
 */
bool getFrontPirActivity() {
  bool active = false;
  byte data[1];
  byte b;
  
  // Get Back PIR
  // I2c.read(ID_PIR_BACK,1,data);
  
  b = mantel.readByteFromSlave(ID_PIR_FRONT);
  active = active || ( b >= motionThreshold );

  if (b != 0) debugSerial.println(F("Front PIR activity detected"));

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
 * Alarm!!
 */
void startAlarm() {
}


