// IDs of the Slaves
#define BACK_ID 20
#define ARM_ID 21
#define BELT_ID 22
#define HEART_ID 30


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
