#define SOUND_SPEED 0.034

#define u32 unsigned int

const int trigPin = 2;
const int echoPin = 4;
const int ledPin = 7;
const int MEASURE_COUNT = 4;

u32 led_last_toggled_at = 0;
const int LED_TOGGLE_INTERVAL_MS = 1000;

u32 sonar_last_fired_at = 0;
const int SONAR_FIRE_INTERVAL_MS = 250;
bool should_light_led = false;

void setup() {
  Serial.begin(9600);

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
  if (ms - sonar_last_fired_at >= SONAR_FIRE_INTERVAL_MS) {
    startSonar();

    float read = sonarRead();

    if (loopIdx == MEASURE_COUNT) {
      float sum = 0;
      int i;
      for (i = 0; i < MEASURE_COUNT; i++) {
        sum += measures[i];
      }

      float avg = sum / MEASURE_COUNT;
      Serial.print("Media movel: ");
      Serial.println(avg);

      // Ligue o LED caso a distância média for acima de 500cm
      should_light_led = avg >= 500.0;

      loopIdx = 0;
    } else {
      measures[loopIdx++] = read;
    }

    sonar_last_fired_at = ms;
  }
}
