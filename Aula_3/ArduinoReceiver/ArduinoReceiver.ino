const int ledPin = LED_BUILTIN;
const int inputPin = 5;

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

void loop() {
  delay(150);
  
  const unsigned int bit = digitalRead(inputPin);
  Serial.print("Valor do pino de entrada: ");
  Serial.println(bit);

  digitalWrite(ledPin, bit);
}
