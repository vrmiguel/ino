const int outputPin = 1;

void setup() {
  pinMode(outputPin, OUTPUT);
  
  digitalWrite(outputPin, HIGH);
}

void loop() {
  delay(2500);
  digitalWrite(outputPin, HIGH);
}
