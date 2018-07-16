/** 
 * This sketch contrrols the motor.
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
#include <Servo.h>
#include <EEPROM.h>

// The pins for the Motor
#define MOTOR_PIN 10
#define MOTOR_SPEED 20
#define MOTOR_DUR_UP 4000
#define MOTOR_DUR_DOWN 4000
#define MOTOR_DUR_ADMIN 100

// The id of the flora
int id = ID_MOTOR;
unsigned long motor_start;
int state;
boolean deactivated;

// Signals
volatile boolean motorUp;
volatile boolean motorDown;
volatile boolean motorStop;

// States
#define STATE_DOWN 0
#define STATE_UP 1
#define STATE_RUNNING_UP 2
#define STATE_RUNNING_DOWN 3
#define STATE_ADMIN 4
#define STATE_DEACT 5

// EEPROM addresses
int isUpAddress = 0;   // Address for the location
int durAddress = 1;    // Address for the duration
int deactAddress = 2;

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

// The servo
Servo servo;

/**
 * Set the Flora up.
 */
void setup() {
  // Attach the servo to MOTOR_PIN
  servo.attach(MOTOR_PIN);
  servo.write(90);

  // Start the i2c conection
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  delay(1000);

  // Start the serial connection for debugging.
  Serial.begin(9600);

  Serial.print(F("Listening on I2C with ID "));
  Serial.println(id);
  Serial.println();
  Serial.println(F("admin: Startet admin Modus"));
  
  motorUp = false;
  motorDown = false;
  motorStop = false;

  EEPROM.get(deactAddress,deactivated);
  
  EEPROM.get(isUpAddress, state);
  if ( state == STATE_UP ) {
     Serial.println(F("Current state is UP"));
  } else if ( state == STATE_DOWN ) {
     Serial.println(F("Current state is DOWN"));
  } else {
     Serial.println(F("Current state is UNDEFINED !!!"));
     servo.write(90);
  }

  Serial.flush();
  
}


/**
 * The main loop does nothing.
 */
void loop() {

  // First check for admin commands
  char buf[20];
  String adminLine;
  if ( Serial.available() ) {
    int len = Serial.readBytesUntil('\n',buf,20);
    adminLine = "";
    for ( int i = 0; i < len ; i++ ) {
      adminLine = adminLine + buf[i];
    }
    Serial.println("> " + adminLine);
  }

  // Start admin mode
  //
  // In admin mode the signals from screen is ignored
  if ( adminLine.equalsIgnoreCase("ADMIN") ) {
    state = STATE_ADMIN;
    servo.write(90);
    showAdminHelp();
  }

  // Show help
  if ( adminLine.equalsIgnoreCase("HELP") ) {
    showAdminHelp();
  }
  
  switch (state) {
    case STATE_UP:
      if ( !deactivated ) handleUp(adminLine);
      break;
    case STATE_DOWN:
      if ( !deactivated ) handleDown(adminLine);
      break;
    case STATE_RUNNING_UP:
      if ( !deactivated ) handleRunningUp();
      break;
    case STATE_RUNNING_DOWN:
      if ( !deactivated ) handleRunningDown();
      break;
    case STATE_ADMIN:
      handleAdmin(adminLine);
      break;
  }
}

// Handle STATE_ADMIN
void handleAdmin(String command) {
  unsigned long now = millis();
  if ( now - motor_start >= MOTOR_DUR_ADMIN ) {
    // Stop motor
    servo.write(90);
  }
  
  if ( command.equalsIgnoreCase("set up") ) {
    servo.write(90);
    deactivated = false;
    EEPROM.put(deactAddress,false);
    EEPROM.put(isUpAddress,STATE_UP);
    state = STATE_UP;
    printState();
  } else if ( command.equalsIgnoreCase("set down") ) {
    servo.write(90);
    deactivated = false;
    EEPROM.put(deactAddress,false);
    EEPROM.put(isUpAddress,STATE_DOWN);
    state = STATE_DOWN;    
    printState();
  } else if ( command.equalsIgnoreCase("stop") ) {
    servo.write(90);    
    printState();
  } else if ( command.equalsIgnoreCase("up") ) {
    motor_start = millis();
    servo.write(90 - MOTOR_SPEED);
    printState();
  } else if ( command.equalsIgnoreCase("down") ) {
    motor_start = millis();
    servo.write(90 + MOTOR_SPEED);    
    printState();
  } else if ( command.equalsIgnoreCase("deact") ) {
    deactivated = true;
    EEPROM.put(deactAddress,true);
    EEPROM.put(isUpAddress,STATE_DEACT);
    state = STATE_DEACT;    
    servo.write(90);    
    printState();
  }
}


// Handle STATE_UP
void handleUp(String command) {
   if ( motorUp ) {
     motorUp = false;
     // Do nothing
   } else if ( motorDown || (command.equalsIgnoreCase("_down_") ) ) {
     motorDown = false; 
     // Start the motor
     state = STATE_RUNNING_DOWN;
     motor_start = millis();    
     servo.write(90 + MOTOR_SPEED);
     printState();
   }
}

// Handle STATE_DOWN
void handleDown(String command) {
   if ( motorDown ) {
     motorDown = false;
     // Do nothing
   } else if ( motorUp  || (command.equalsIgnoreCase("_up_") )) {
     motorUp = false; 
     // Start the motor
     state = STATE_RUNNING_UP;
     motor_start = millis();    
     servo.write(90 - MOTOR_SPEED);
     printState();
   }
}

// Handle STATE_RUNNING_UP
void handleRunningUp() {
  unsigned long now = millis();
  // Stop motor after given duration
  if ( ( now - motor_start ) >= MOTOR_DUR_UP ) {
    servo.write(90);
    state = STATE_UP;
    EEPROM.put(isUpAddress,STATE_UP);
    printState();
  }
}


// Handle STATE_RUNNING_DOWN
void handleRunningDown() {
  unsigned long now = millis();
  // Stop motor after given duration
  if ( ( now - motor_start ) >= MOTOR_DUR_DOWN ) {
    servo.write(90);
    state = STATE_DOWN;
    EEPROM.put(isUpAddress,STATE_DOWN);
    printState();
  }
}

// Show the admin commands
void showAdminHelp() {
  Serial.println();
  printState();
  Serial.println(F("Admin Kommandos:"));
  Serial.println(F("stop     : Stoppe den Motor"));
  Serial.println(F("up       : Lasse den Motor hoch laufen"));
  Serial.println(F("down     : Lasse den Motor runter laufen"));
  Serial.println(F("set up   : Beende Admin Modus und markiere als obere Position"));
  Serial.println(F("set down : Beende Admin Modus und markiere als untere Position"));
  Serial.println(F("deact    : Beende Admin Modus mit deaktivierem Motor"));
  Serial.println();
  Serial.flush();
}


void printState() {
  if ( deactivated ) {
    Serial.println(F("DEAKTIVIERT!"));
    return;
  }
  
  Serial.print(F("Aktueller Zustand: "));
  switch (state) {
    case STATE_UP:
      Serial.println(F("UP"));
      break;
    case STATE_DOWN:
      Serial.println(F("DOWN"));
      break;
    case STATE_RUNNING_UP:
      Serial.println(F("RUNNING UP"));
      break;
    case STATE_RUNNING_DOWN:
      Serial.println(F("RUNNING DOWN"));
      break;
    case STATE_ADMIN:
      Serial.println(F("ADMIN"));
      break;
    case STATE_DEACT:
      Serial.println(F("DEAKTIVIERT"));
      break;
  }
  Serial.flush();
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

  // Serial.print("Received cmd: ");
  // Serial.print(cmd);
  //Serial.print(" dlen: ");
  // Serial.println(dlen);

  mantel.readData(dlen,data);
  
  switch (cmd) {
   case CMD_MOTOR_STOP:
      motorStop = true;
      break;
    case CMD_MOTOR_UP:
      motorUp = true;
      break;
    case CMD_MOTOR_DOWN:
      motorDown = true;
      break;
  }
  mantel.emptyWire();
}


