#include <Wire.h>
#include <HiTechMantel.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_NeoPixel.h>

#define screenSerial Serial1
#define debugSerial Serial

#define PULSE_PIN A0
#define PULSE_INTERVAL 500
#define PULSE_AVG_COUNT 10

#define SLEEPTIME 300000 // 5 min


unsigned long lastActivity;
bool sleeping = false;
bool measuringPulse = false;
int pulseMin;
int pulseMax;
int pulseMid;
int pulseTick;
bool pulse;
long pulseStart;
long pulseDur;


HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;


// Read a byte from the screen
byte readByteFromScreen() {
  byte b = screenSerial.available() ? screenSerial.read() : 0;
  delay(1);
  return b;
}

/**
 * Read bytes from the screen
 */
void readBytesFromScreen(int len, byte buf[]) {
  for (int i = 0; i < len; i++) {
    buf[i] = readByteFromScreen();
  }
}

/**
 * send a byte to i2c
 */
void sendByte(byte id, byte data) {
  Wire.beginTransmission(id);
  Wire.write(data);
  Wire.endTransmission();
}

/**
 * Send an array of bytes via i2c
 */
void sendArray(byte id, byte data[], int length) {
  Wire.beginTransmission(id);
  Wire.write(data,length);
  Wire.endTransmission();
  delay(100);
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
  Wire.begin();

  // Initialize serial connections
  debugSerial.println("Init done");
  debugSerial.println("Listen as master");

  lastActivity = millis();
  sleeping = false;
}


/**
 * Recevie messages fromthe screen
 */
void loop() {
  bool flag = false;
  byte data[255];

  if ( sleeping ) {
    bool active = getPirActivity();
    if (active) wakeUp();
  } else if (screenSerial.available() ) {
      lastActivity = millis();
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
          debugSerial.println("Processing command myself");
          switch (cmd) {
             case CMD_PULSE_START:
               Serial.println("Starting pulse measurement");
               pulseMin = 512;
               pulseMid = 512;
               pulseMax = 512;
               pulse = false;
               pulseTick = 0;
               measuringPulse = true;
               break;
             case CMD_PULSE_STOP:
               Serial.println("Ending pulse measurement");
               measuringPulse = false;
               break;            
          }
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
    } else if ( (millis() - lastActivity ) > SLEEPTIME ) {
      // Check PIRS
      // if ( !getPirActivity() ) {
        // Go to sleep
        goToSleep();  
      // }
  }
  delay(2);
}

/**
 * Measure the pulse and display it on screen.
 */
void measurePulse() {
  pulseTick++;
  
  int pulseRaw = analogRead(PULSE_PIN);

  if ( pulseRaw < pulseMin ) pulseMin = pulseRaw;
  if ( pulseRaw > pulseMax ) pulseMax = pulseRaw;
  pulseMid = (pulseMax - pulseMin)/2;

  if ( (pulseRaw > pulseMid) && (pulse == false) ) {
    pulse = true;
    pulseDur = millis() - pulseStart;
    pulseTick = 0;
    pulseStart = millis();
  } else if ( (pulseRaw < pulseMid) && (pulse == true) ) {
    pulse = false;
  }
  
  int pulseValue = map(pulse,pulseMin,pulseMax,0,255);

  screenSerial.print("t0.txt=\"");
  screenSerial.print(pulseValue);
  screenSerial.print("\"");
  screenSerial.write(0xff);
  screenSerial.write(0xff);
  screenSerial.write(0xff);  
  screenSerial.print("add 2,0,");
  screenSerial.print(pulseValue);
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
  sendByte(ID_BACK,0);
  sendByte(ID_ARM,0);
  sendByte(ID_BELT,0);
  sendByte(ID_STRIP,0);
  sendByte(ID_MATRIX,0);
  sendByte(ID_MP3,0);
  sendByte(ID_MOTOR,0);
  // Send sleep signal to screen
  sendToScreen(F("page 42"));
}

/**
 * Check both PIRS for activity in the last perios of time
 */
bool getPirActivity() {
  bool active = false;
  
  // Get Front PIR
  Wire.requestFrom(ID_PIR_FRONT,1);

  while (Wire.available()) {
    byte b = Wire.read();
    active = active || ( b != 0 );
  }

  // Get Back PIR
  Wire.requestFrom(ID_PIR_BACK,1);

  while (Wire.available()) {
    byte b = Wire.read();
    active = active || ( b != 0 );
  }

  return active;
}


