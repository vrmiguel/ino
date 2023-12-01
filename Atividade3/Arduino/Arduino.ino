#include <Adafruit_BMP280.h>
#include "DHT.h"

int last_updated_sensors = 0;
int sensor_update_interval = 1000;

const int MEASURE_COUNT = 5;

const int led_pin = LED_BUILTIN;
const int ldr_pin = A0;

Adafruit_BMP280 bmp;

// "Limiar de escuridão" do LDR (sensor de luminância)
// Se o valor lido for acima deste limiar, considere que o sensor está
// em um cenário de pouca luz
const int darkness_threshold = 700;

void setup() {
  Serial.begin(9600);

  pinMode(led_pin, OUTPUT);
  if (!bmp.begin(0x76)) {
    for (;;) {
      Serial.println("BMP nao ligou");
      delay(10);
    }
  }
  Serial2.begin(9600);
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
  int bmp_temp_avg[MEASURE_COUNT] = {0};

  void loop() {
    int ms = millis();
    if (ms - last_updated_sensors >= sensor_update_interval) {
      last_updated_sensors = ms;

      // Lê a luminância do LDR
      int lumens = analogRead(ldr_pin);
      push_arr(ldr_lumen_avg, lumens);

      int temp = bmp.readTemperature();
      push_arr(bmp_temp_avg, temp);
    }

    if (Serial2.available() > 0) {
      const char msg = Serial2.read();
      if(msg == '2') {      
        const int lumen_avg = get_average(ldr_lumen_avg);
        Serial2.print(lumen_avg);
        
        Serial.print("Enviei ");
        Serial.print(lumen_avg);
        Serial.println("Lumens ao ESP32");
      } else if (msg == '1') {
        const int temp_avg = get_average(bmp_temp_avg);
        Serial2.print(temp_avg);
        
        Serial.print("Enviei ");
        Serial.print(temp_avg);
        Serial.println("C ao ESP32");
      }
    }
  }
  
