
const int MEASURE_COUNT = 5;

const int motion_sensor_pin = 3;
const int rain_sensor_pin = 5;
const int lm35_pin = A1;
const int ldr_pin = A0;

int last_updated_sensors = 0;
int sensor_update_interval = 5000;

int is_raining;
int is_near;

int temperature_avg[MEASURE_COUNT] = {0};
int luminosity_avg[MEASURE_COUNT] = {0};

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  pinMode(motion_sensor_pin, INPUT);
  pinMode(rain_sensor_pin, INPUT);

  Serial.println("Iniciado!");
}

void push_arr(int * arr, int new_value) {
  // Shift array
  for (int i = MEASURE_COUNT - 1; i > 0; i--) {
    arr[i] = arr[i - 1];
  }
  arr[0] = new_value;
}

int get_average(const int * arr) {
  int sum = 0;
  for (int i = 0; i < MEASURE_COUNT; i++) {
    sum += arr[i];
  }

  return sum / MEASURE_COUNT;
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
  int ms = millis();
  if (ms - last_updated_sensors >= sensor_update_interval) {
    Serial.println("Atualizando sensores!");
    // Atualização de sensores

    is_raining = digitalRead(rain_sensor_pin);
    Serial.print("Chove?");
    Serial.println(is_raining ? "Sim" : "Nao");
    
    is_near = digitalRead(motion_sensor_pin);
    Serial.print("Motorista no carro?");
    Serial.println(is_near ? "Sim" : "Nao");

    
    int lumen_reading = analogRead(ldr_pin);
    Serial.print("Luminancia:");
    Serial.println(lumen_reading);

    
    int temp_reading = lm35_read_temperature();
    Serial.print("Temperatura:");
    Serial.println(temp_reading);

    push_arr(luminosity_avg, lumen_reading);
    push_arr(temperature_avg, temp_reading);

    

    last_updated_sensors = ms;
  }

  if (Serial2.available()) {
    Serial.println("Pedido do ESP32 recebido");
    char chooser = Serial2.read();

    if (chooser == 't') {
      // t -> temperature
      
      String temperature = String(get_average(temperature_avg), DEC);
      Serial2.print(temperature);
      Serial.print(temperature);
    } else if (chooser == 'r') {
      // r -> rain sensor
      Serial2.print(is_raining);
      Serial.print(is_raining);
    } else if (chooser == 'm') {
      // m -> motion sensor
      Serial2.print(is_near);
      Serial.print(is_near);      
    } else if (chooser == 'l') {
      // l -> lumens
      String luminosity = String(get_average(luminosity_avg), DEC);
      Serial2.print(luminosity);
      Serial.print(luminosity);
    }
  }
}