int pulseSensorPin = 9;
int led = 8;

int signalIn;
int threshold = 550;

int count = 1;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
}

void loop() {

  digitalWrite(led, HIGH);

  signalIn = analogRead(pulseSensorPin);
  threshold = (threshold + signalIn) / count;

  Serial.println(signalIn);

  if (threshold > 1000) {
    if ((signalIn > threshold)) {
      digitalWrite(led, HIGH);
    } else {
      digitalWrite(led, LOW);
    }
  }

  delay(10);


}
