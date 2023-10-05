const int led_pin = LED_BUILTIN;
const int darkness_threshold = 700;

void setup() {
  pinMode(led_pin, OUTPUT);

  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  // Lê a luminância do LDR
  int lumens = analogRead(A0);
  Serial.print(lumens);
  Serial.println(" lumens");

  // Se o ESP32 nos enviar um sinal,
  // ligar o LED do Arduino
  if (Serial2.available()) {
    const bool should_light = Serial2.read() == 1;
    const int state = should_light ? HIGH : LOW;
    digitalWrite(led_pin, should_light);
  }

  if (lumens > darkness_threshold) {
    // Tá no escuro
    Serial.println("Sensor esta' no escuro, enviando HIGH");
    Serial2.write(1);
  } else {
    // Não tá no escuro kkk
    Serial.println("Sensor esta' na \"luz\"");
  }
}
