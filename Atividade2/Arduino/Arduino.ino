#include <Adafruit_BMP280.h>
#include "DHT.h"

int last_updated_sensors = 0;
int sensor_update_interval = 1000;

const int MEASURE_COUNT = 5;

const int led_pin = LED_BUILTIN;
const int dht_pin = 2;
const int ldr_pin = A0;
const int lm35_pin = A1;

Adafruit_BMP280 bmp;

// Configurando o DHT11 para uso
DHT dht(dht_pin, DHT11);

// "Limiar de escuridão" do LDR (sensor de luminância)
// Se o valor lido for acima deste limiar, considere que o sensor está
// em um cenário de pouca luz
const int darkness_threshold = 700;

void setup() {
  Serial.begin(9600);

  pinMode(led_pin, OUTPUT);
  pinMode(dht_pin, INPUT);
  dht.begin();
  if (!bmp.begin(0x76)) {
    for (;;) {
      Serial.println("BMP nao ligou");
      delay(10);
    }
  }
  Serial2.begin(9600);
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

void shift_array(int * arr) {
  for (int i = MEASURE_COUNT - 1; i > 0; i--) {
    arr[i] = arr[i - 1];
  }
}

void push_arr(int * arr, int new_value) {
  shift_array(arr);
  arr[0] = new_value;
}

int get_average(const int * arr) {
  int sum = 0;
  for (int i = 0; i < MEASURE_COUNT; i++) {
    sum += arr[i];
  }

  return sum / MEASURE_COUNT;
}

  void show_arr(const int * arr) {
    Serial.print("[");
    for (int i = 0; i < MEASURE_COUNT; i++) {
      Serial.print(arr[i]);
      Serial.print(",");
    }
    Serial.println("]");
  }

  int ldr_lumen_avg[MEASURE_COUNT] = {0};
  int dht_humidity_avg[MEASURE_COUNT] = {0};
  int dht_temp_avg[MEASURE_COUNT] = {0};
  int bmp_temp_avg[MEASURE_COUNT] = {0};
  int bmp_pressure_avg[MEASURE_COUNT] = {0};
  int bmp_altitude_avg[MEASURE_COUNT] = {0};
  int lm35_temp_avg[MEASURE_COUNT] = {0};

  void loop() {
    int ms = millis();
    if (ms - last_updated_sensors >= sensor_update_interval) {
      last_updated_sensors = ms;

      // Lê a luminância do LDR
      int lumens = analogRead(ldr_pin);
      push_arr(ldr_lumen_avg, lumens);

      // Umidade do DHT
      int humi_dht = dht.readHumidity();
      push_arr(dht_humidity_avg, humi_dht);

      int tempC_dht = dht.readTemperature();
      push_arr(dht_temp_avg, tempC_dht);

      int tempC_bmp = bmp.readTemperature();
      push_arr(bmp_temp_avg, tempC_bmp);

      int press_bmp = bmp.readPressure();
      push_arr(bmp_pressure_avg, press_bmp);

      int alt_bmp   = bmp.readAltitude();
      push_arr(bmp_altitude_avg, alt_bmp);

      int lm35_temp = lm35_read_temperature();
      push_arr(lm35_temp_avg, lm35_temp);
    }

    if (Serial2.available()) {    
      char chooser = Serial2.read();
      Serial.print("Li ");
      Serial.print(chooser);
      Serial.println(" do Serial2");
      
      if (chooser == '1') {
        int avg = get_average(lm35_temp_avg);
        Serial2.write(avg);
        Serial.print("Mandei ");
        Serial.print(avg);
        Serial.println("C para o ESP32");

        Serial.println("Temperatura do LM35");
        show_arr(lm35_temp_avg);


      } else if (chooser == '2') {
        int avg = get_average(ldr_lumen_avg);
                
        String lumens = String(avg, DEC); 
        Serial2.print(lumens);

        Serial.print("Mandei ");
        Serial.print(avg);
        Serial.println(" lumens para o ESP32");

        Serial.println("Lumens do LDR");
        show_arr(ldr_lumen_avg);
            
      }
    }
  }
  
