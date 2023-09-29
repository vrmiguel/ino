#define u32 unsigned int

int val = 0;

const int touch_gpio = 13;
const int buzzer_gpio = 27;
const int arduino_output_gpio = 14;
const int arduino_input_gpio = 5;


const int touch_threshold = 12;

u32 consecutive_touches = 0;
bool should_read_arduino = false;

u32 touch_last_read_at = 0;
const int TOUCH_READ_INTERVAL_MS = 100;

bool got_touched() {
  long new_count = touchRead(touch_gpio);

  bool was_touched = new_count < touch_threshold;

  return was_touched;
}

void setup() {
  Serial.begin(9600);

  pinMode(buzzer_gpio, OUTPUT);
  pinMode(arduino_output_gpio, OUTPUT);
  pinMode(arduino_input_gpio, INPUT);
}

void loop() {
  if (should_read_arduino) {
    const int value = digitalRead(arduino_input_gpio);
    if (value == HIGH) {
      Serial.println("Ativando buzzer");
      digitalWrite(buzzer_gpio, HIGH);
    } else {
      Serial.println("Li do Arduino e era LOW");
      digitalWrite(buzzer_gpio, LOW);
    }

    should_read_arduino = false;
  }

  const u32 now = millis();
  if (now - touch_last_read_at >= TOUCH_READ_INTERVAL_MS) {
    touch_last_read_at = now;
    if (got_touched()) {
      consecutive_touches += 1;
    } else {
      consecutive_touches = 0;
      digitalWrite(arduino_output_gpio, LOW);
    }

    if (consecutive_touches >= 5) {
      Serial.println("Enviando HIGH ao Arduino para fazer a consulta");
      digitalWrite(arduino_output_gpio, HIGH);
      should_read_arduino = true;
      consecutive_touches = 0;
    }


    Serial.print("Toques consecutivos: ");
    Serial.println(consecutive_touches);
  }
}
