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

// FEEDBACK settings
#define FEEDBACK true
#define FB_UNITS 72
#define FB_THRESHOLD 60
#define FB_PIN 12
#define FB_BASE_POS 0
#define FB_TURNS 6
#define FB_DC_MIN 0.029
#define FB_DC_MAX 0.971

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

  if ( FEEDBACK ) {
    pinMode(FB_PIN,INPUT);
    Serial.println("Running in FEEDBACK MODE");
    Serial.print("Setting motor to base position ... ");
    Serial.flush();  
    moveTo(FB_BASE_POS,5);
    Serial.println("reached");
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

  if (!FEEDBACK) {
    // Call the handler for the current state
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
  } else {
    // Call the handler for the current state
    switch (state) {
      case STATE_UP:
        if ( !deactivated ) handleUpFeedback(adminLine);
        break;
      case STATE_DOWN:
        if ( !deactivated ) handleDownFeedback(adminLine);
        break;
      case STATE_RUNNING_UP:
        if ( !deactivated ) handleRunningUpFeedback();
        break;
      case STATE_RUNNING_DOWN:
        if ( !deactivated ) handleRunningDownFeedback();
        break;
      case STATE_ADMIN:
        handleAdminFeedback(adminLine);
        break;
    }    
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
  if ( FEEDBACK ) Serial.println("FEEDBACK MODE");  
  Serial.println();
  Serial.flush();
}


void printState() {
  if ( deactivated ) {
    Serial.println(F("DEAKTIVIERT!"));
    return;
  }

  if ( FEEDBACK ) Serial.println("FEEDBACK MODE");
  
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

/**
 * Feedback operations
 */


/**
 * Get the current degree of the servo
 */
int getDegree() {
   int tHigh;
   int tLow;
   int tCycle;

   float theta = 0;
   float dc = 0;
   float dutyScale = 1;

   while(1) {
     pulseIn(FB_PIN, LOW);
     tHigh = pulseIn(FB_PIN, HIGH);
     tLow =  pulseIn(FB_PIN, LOW);
     tCycle = tHigh + tLow;
     if ( (tCycle > 1000) && ( tCycle < 1200)) break;
   } 

   dc = (dutyScale * tHigh) / tCycle;
   theta = ((dc - FB_DC_MIN) * FB_UNITS ) / (FB_DC_MAX - FB_DC_MIN);
   return theta;  
}

/**
 * Move to the target position. Choose the shorter direction
 */
void moveTo(int target, int speed) {
  int pos = getDegree();
  int delta = pos - target; // number of degrees in clockwise direction
  
  Serial.print(pos);
  Serial.print(" ");
  Serial.print(target);
  Serial.print(" ");
  Serial.println(delta);
  Serial.flush();

  
  if (delta > FB_UNITS/2) {
    // counterclockwise is shorter
    delta = delta - FB_UNITS;
  } else if ( delta < - (FB_UNITS/2)) {
    // clockwise is shorter
    delta = FB_UNITS + delta;
  }
  Serial.println(delta);
  Serial.flush();
  
  if ( delta > 0 ) { // clockwise
    servo.write(90 + speed);      
  } else { // counterclockwise
    servo.write(90 - speed);           
  }

  int theta;
  while ( (theta = getDegree()) != target ) {
  }
  servo.write(90);  
  delay(50);
}



void clockwise(int ticks, int speed) {
  int curSpeed = speed;
  int remain = ticks;
  int oldValue = getDegree();
  oldValue = getDegree();
  int target = (oldValue + ticks) % FB_UNITS; 
  int newValue;
  int delta;

  while ( remain > 1 ) {
    servo.write(90 - curSpeed);
    newValue = getDegree();
    delta = newValue - oldValue;
    oldValue = newValue;
    if ( delta < - FB_THRESHOLD ) {
      delta += FB_UNITS;
    }

    if ( delta > ( (1.0 * remain) / 20.0 ) ) {
      curSpeed = (3 * curSpeed) / 5;
      if (curSpeed < 10) curSpeed = 10;
    }

    remain = remain - delta;
  }
  
  servo.write(90);
  delay(50);
}


void counterclockwise(int ticks, int speed) {
  int curSpeed = speed;
  int remain = ticks;
  int oldValue = getDegree();
  int target = (oldValue - ticks) % FB_UNITS;
  while ( target < 0 ) target += FB_UNITS; 
  oldValue = getDegree();
  int newValue;
  int delta;

  while ( remain > 1 ) {
    servo.write(90 + curSpeed);
    newValue = getDegree();
    delta = oldValue - newValue;
    oldValue = newValue;
    if ( delta < - FB_THRESHOLD ) {
      delta += FB_UNITS;
    }

    if ( delta > ((1.0 * remain) / 20.0 ) ) {
      curSpeed = (3 * curSpeed) / 5;
      if (curSpeed < 10) curSpeed = 10;
    }

    remain = remain - delta;
  }

  servo.write(90);
  delay(50);
}



// Handle STATE_ADMIN in feedback mode
void handleAdminFeedback(String command) {
  
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
    // One turn up
    clockwise(1*FB_UNITS,MOTOR_SPEED);
    moveTo(FB_BASE_POS,5);
    printState();
  } else if ( command.equalsIgnoreCase("down") ) {
    // One turn down
    counterclockwise(1*FB_UNITS,MOTOR_SPEED);
    moveTo(FB_BASE_POS,5);
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
void handleUpFeedback(String command) {
   if ( motorUp ) {
     // Do nothing if command motorUp received
     motorUp = false;
   } else if ( motorDown || (command.equalsIgnoreCase("_down_") ) ) {
     motorDown = false; 
     // Let the motor run downwards
     counterclockwise(FB_TURNS*FB_UNITS,MOTOR_SPEED);
     moveTo(FB_BASE_POS,5);
     state = STATE_DOWN;
     printState();
     // Ignore all commands received in the mean time
     motorUp = false;
     motorDown = false;
     motorStop = false;
   }
}

// Handle STATE_DOWN
void handleDownFeedback(String command) {
   if ( motorDown ) {
     motorDown = false;
     // Do nothing
   } else if ( motorUp  || (command.equalsIgnoreCase("_up_") )) {
     motorUp = false; 
     // Let the motor run upwards
     clockwise(FB_TURNS*FB_UNITS,MOTOR_SPEED);
     moveTo(FB_BASE_POS,5);
     state = STATE_UP;
     printState();
     // Ignore all commands received in the mean time
     motorUp = false;
     motorDown = false;
     motorStop = false;
   }
}

// Handle STATE_RUNNING_UP
void handleRunningUpFeedback() {
  // In feedback mode this is ignored
}


// Handle STATE_RUNNING_DOWN
void handleRunningDownFeedback() {
  // In feedback mode this is ignored
}

