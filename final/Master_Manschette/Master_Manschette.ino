#include <Wire.h>
#include <HiTechMantel.h>


// Send an RGB-color to the given id and register
void sendRGB(char id, char reg, char red, char green, char blue) {
  Wire.beginTransmission(id);
  Wire.write(reg);
  Wire.write(red);
  Wire.write(green);
  Wire.write(blue);
  Wire.endTransmission();
}



void setup() {
  Wire.begin();
}


void loop() {
}
