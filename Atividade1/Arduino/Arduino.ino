#define SOUND_SPEED 0.034

#define u32 unsigned int

const int trigPin = 2;
const int echoPin = 4;
const int ledPin = LED_BUILTIN;
const int MEASURE_COUNT = 4;

const float distance_threshold_cm = 20.0;

const int esp32_input_gpio = 7;
const int esp32_output_gpio = 5;

bool ledState = HIGH;
u32 led_last_toggled_at = 0;
const int LED_TOGGLE_INTERVAL_MS = 1000;

u32 sonar_last_fired_at = 0;
const int SONAR_FIRE_INTERVAL_MS = 250;
bool should_light_led = false;

void setup() {
  Serial.begin(9600);

  pinMode(esp32_output_gpio, OUTPUT);

  pinMode(esp32_input_gpio, INPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  delay(100);
}

void startSonar() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

// Lê a distância em centímetros
float sonarRead() {
  const long duration = pulseIn(echoPin, HIGH);
  const float distanceCm = duration * SOUND_SPEED / 2;

  return distanceCm;
}


int loopIdx = 0;
float measures[MEASURE_COUNT];

void loop() {
  u32 ms = millis();

  if (should_light_led) {
    if (ms - led_last_toggled_at >= LED_TOGGLE_INTERVAL_MS) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      led_last_toggled_at = ms;
    }
  } else if (ledState) {
    // Se estamos aqui, então should_light_led == false
    // Se o LED ainda está aceso, portanto, o mesmo deverá ser apagado
    ledState = LOW;
    digitalWrite(ledPin, ledState);
  }

  if (ms - sonar_last_fired_at >= SONAR_FIRE_INTERVAL_MS) {
    startSonar();

    float read = sonarRead();

    if (digitalRead(esp32_input_gpio) == HIGH) {
      Serial.println("ESP32 pediu status do SR-HC04");
      const int to_send = read >= distance_threshold_cm ? HIGH : LOW;
      digitalWrite(esp32_output_gpio, to_send);
    }

    if (loopIdx == MEASURE_COUNT) {
      float sum = 0;
      int i;
      for (i = 0; i < MEASURE_COUNT; i++) {
        sum += measures[i];
      }

      float avg = sum / MEASURE_COUNT;
      Serial.print("Media movel: ");
      Serial.println(avg);

      // Ligue o LED caso a distância média for acima de distance_threshold_cm
      should_light_led = avg >= distance_threshold_cm;

      Serial.print("Devemos acender o LED?: ");
      Serial.println(should_light_led);

      loopIdx = 0;
    } else {
      measures[loopIdx++] = read;
    }

    sonar_last_fired_at = ms;
  }

  const int esp32in = digitalRead(esp32_input_gpio);
  Serial.print("Entrada do ESP32: ");
  Serial.println(esp32in);

  const int esp32out = digitalRead(esp32_output_gpio);
  Serial.print("Saida do ESP32: ");
  Serial.println(esp32out);
}