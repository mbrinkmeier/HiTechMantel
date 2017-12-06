/**
 * This sketch is the programm for the thre RGB controlling Floras.
 * 
 * It receives the red, green and blue component to be displayed.
 */
#include <Wire.h>
#include <HiTechMantel.h>
#include <Servo.h>

// The pins for the Motor
#define MOTOR_PIN 10
#define MOTOR_SPEED 20
#define MOTOR_DUR 20000

// The id of the flora
int id = ID_MOTOR;
unsigned long motor_start;

HiTechMantel mantel = HiTechMantel();
Adafruit_NeoPixel pixel = mantel.pixel;

// The servo
Servo servo;

/**
 * Set the Flora up.
 */
void setup() {
  // Start the serial connection for debugging.
  Serial.begin(9600);

  // Attach the servo to MOTOR_PIN
  servo.attach(MOTOR_PIN);

  // Start the i2c conection
  Wire.begin(id);               // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  Serial.print("Listening on I2C with ID ");
  Serial.println(id);
}


/**
 * The main loop does nothing.
 */
void loop() {
  if ( millis() - motor_start > MOTOR_DUR ) {
    Serial.println("Stopping Motor due to timeout");
    servo.write(90);
    motor_start = millis();
  }
  delay(100);
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

  Serial.print("Received cmd: ");
  Serial.print(cmd);
  Serial.print(" dlen: ");
  Serial.println(dlen);

  mantel.readData(dlen,data);
  
  switch (cmd) {
   case CMD_MOTOR_STOP:
      servo.write(90);
      break;
    case CMD_MOTOR_UP:
      servo.write(90-MOTOR_SPEED);
      motor_start = millis();
      break;
    case CMD_MOTOR_DOWN:
      servo.write(90+MOTOR_SPEED);
      motor_start = millis();
      break;
   case CMD_PING:
      Wire.onRequest(waitForPing);
      break;
  }
}



/**
 * Wait for the ping request.
 */
void waitForPing() {
  Wire.write("ping");
  Wire.onRequest(NULL);
}

