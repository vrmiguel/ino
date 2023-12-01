const int MEASURE_COUNT = 5;

const int motion_sensor_pin = A0;
const int rain_sensor_pin = A1;
const int lm35_pin = 2;
const int ldr_pin = 3;

int last_updated_sensors = 0;
int sensor_update_interval = 1000;

int is_raining;
int is_near;
int lumens;
float temp_celsius;

int temperature_avg[MEASURE_COUNT] = {0};
int luminosity_avg[MEASURE_COUNT] = {0};

void setup() {
  Serial.begin(9600);

  pinMode(motion_sensor_pin, INPUT);
}

void push_arr(int * arr, int new_value) {
  // Shift array
  for (int i = MEASURE_COUNT - 1; i > 0; i--) {
    arr[i] = arr[i - 1];
  }
  arr[0] = new_value;
}


float lm35_read_volts() {
  int step_value = analogRead(lm35_pin);
  float step_value_f = (float) step_value;

  const float volts = step_value_f * (5.0 / 1024.0);
  return volts;
}

float lm35_read_temperature() {
  // Leia a voltagem convertendo pra mV
  const float v_in = lm35_read_volts() * 1000.0;

  // T = 10 * v_in
  return v_in / 10;
}

void loop() {
  delay(2500);

  int ms = millis();
  if (ms - last_updated_sensors >= sensor_update_interval) {
    // Atualização de sensores

    int is_raining = digitalRead(rain_sensor_pin);
    int is_near = digitalRead(motion_sensor_pin);
    int lumen_reading = analogRead(ldr_pin);
    int temp_reading = lm35_read_temperature();
    
    push_arr(luminosity_avg, lumen_reading);
    push_arr(temperature_avg, temp_reading);
  }

  if (Serial2.available()) {
    char chooser = Serial2.read();

    if (chooser == 't') {
      // t -> temperature
      String temperature = String(temp_celsius, DEC);
      Serial2.print(temperature);
    } else if (chooser == 'r') {
      // r -> rain sensor
      Serial2.print(is_raining);
    } else if (chooser == 'm') {
      // m -> motion sensor
      Serial2.print(is_near);      
    } else if (chooser == 'l') {
      // l -> lumens
      String luminosity = String(lumens, DEC);
      Serial2.print(luminosity);
    }
  }
}
