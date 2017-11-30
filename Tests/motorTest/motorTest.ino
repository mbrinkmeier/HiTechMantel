/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int val = 20;    // variable for the spped of the servo


void setup() {
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  myservo.write(90+val);  // sets the servo position according to the scaled value
  delay(2000); 
  myservo.write(90);  // sets the servo position according to the scaled value
  delay(1000); 
  myservo.write(90-val);  // sets the servo position according to the scaled value
  delay(2000); 
  myservo.write(90);  // sets the servo position according to the scaled value
  delay(1000); 
}

