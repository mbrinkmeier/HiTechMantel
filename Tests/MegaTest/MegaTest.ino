#include <Wire.h>
#include <HiTechMantel.h>

#define screenSerial Serial2

#define PULSE_PIN A0
#define PULSE_INTERVAL 500
#define PULSE_AVG_COUNT 10

bool measuringPulse = false;
int pulseMin;
int pulseMax;
int pulseMid;
int pulseTick;
bool pulse;
long pulseStart;
long pulseDur;

// NexWaveform pulseForm = NexWaveform(6,2,"pulseForm");
// NexTouch *nex_Listen_List[] = { NULL };

void send(byte id, byte data[], int length) {
  Wire.beginTransmission(id);
  Wire.write(data,length);
  Wire.endTransmission();
}


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

void setup() {
  // nexInit();

  // Initialize I2C as master
  Wire.begin();

  // Initialize serial connections
  Serial.begin(9600);
  screenSerial.begin(9600);  
}

void loop() {
  if ( measuringPulse ) {
    measurePulse();  
  }
  
  bool flag = false;
  if (screenSerial.available() ) {
    byte start = screenSerial.read();
    if ( start == START_BYTE ) {
      // Start reading a command
      byte id = screenSerial.read();
      byte cmd = screenSerial.read();
      byte data = screenSerial.read();

      Serial.print("id: ");
      Serial.print(id);
      Serial.print(" cmd: ");
      Serial.print(cmd);
      Serial.print(" data : ");
      Serial.println(data);
      
      // Now treat the commands, depending on their data length
      switch (id) {
        case ID_BACK:
        case ID_ARM:
        case ID_BELT:
           // Extract the colors from the data byte
           byte color[3];
           color[0] = 255 * (data % 2);
           data = data / 2;
           color[1] = 255 * (data % 2);
           data = data / 2;
           color[2] = 255 * (data % 2);
           // Send values
           send(id,color,3);
           break;

        case ID_PULSE:
           switch (cmd) {
             case CMD_PING:
               // respond to a ping
               break;
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
           break;
      }

      
    }
  }
  delay(2);
}
