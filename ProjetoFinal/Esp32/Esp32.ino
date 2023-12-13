#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/*
  const int button_pin = 21;
  const int headlight_led = 1;
  const int rain_led = 3;
*/

const int button_pin = 26;
const int headlight_led = 12;
const int rain_led = 14;

// Variáveis para tratamento de debounce de botão
const unsigned int debounce_delay = 5; // ms
int previous_button_state = LOW;
int button_state;
int last_button_press;

int headlight_override = false;

const char *MQTT_SERVER = "broker.hivemq.com";
const char *MQTT_TOPIC = "iot/uptsi/sensor";

const uint16_t MQTT_PORT = 1883;

const char* ssid = "Crabstation";
const char* password = "senhasenha";

int last_updated_sensors = 0;
int sensor_update_interval = 1000;

int last_updated_mqtt = 0;
int mqtt_update_interval = 1000;

// Dados de sensores, vindos do Arduino
int temperature;
int luminosity;
int is_raining;
int is_near;
int is_dark = false;

void CallbackMqtt(char* topic, byte* payload, unsigned int length) {
  if (String(topic) == "iot/uptsi/botao") {

    bool turn_on_headlight = payload[0] == '1';

    payload[length] = '\0';
    Serial.print("Payload: ");
    Serial.println(String((char*) payload));

    
    Serial.print("Recebi mensagem do topico, payload: ");
    Serial.println(turn_on_headlight);

    if (turn_on_headlight) {
      headlight_override = !headlight_override;
    }
  }
}

String check_sensor(char sensor) {
  if (sensor == 'r') {
    Serial.print("Sensor de chuva: ");
  } else if (sensor == 'm') {
    Serial.print("Sensor de proximidade: ");
  } else if (sensor == 't') {
    Serial.print("Temperatura: ");
  } else if (sensor == 'l') {
    Serial.print("Lumens: ");
  } else {
    Serial.print("Desconhecido");
  }

  String retorno;

  Serial2.print(sensor);

  // Espera o Arduino responder
  while (!Serial2.available()) {}

  retorno = Serial2.readString();

  Serial.println(retorno);

  return retorno;
}

void ConnectToMqtt() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao MQTT, clientID: ");
    char clientId[100] = "\0";
    sprintf(clientId, "ESP32Client-%04X", random(0xffff));
    if (mqttClient.connect(clientId)) {
      Serial.println("Conectado ao broker MQTT");
      mqttClient.subscribe(MQTT_TOPIC);
      mqttClient.subscribe("iot/uptsi/botao");
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  pinMode(headlight_led, OUTPUT);
  pinMode(rain_led, OUTPUT);
  pinMode(button_pin, INPUT_PULLDOWN);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(CallbackMqtt);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Atualize os valores de sensor com o Arduino
void update_sensors() {
  String temp = check_sensor('t');
  Serial.print("Valor lido: ");
  Serial.println(temperature);
  temperature = temp.toInt();

  String lumens = check_sensor('l');
  luminosity = lumens.toInt();
  Serial.print("Valor lido: ");
  Serial.println(luminosity);

  is_dark = luminosity < 500;
  if (headlight_override || is_dark) {
    digitalWrite(headlight_led, HIGH);
  } else {
    digitalWrite(headlight_led, LOW);
  }

  String rain = check_sensor('r');
  is_raining = rain.toInt();
  Serial.print("Valor lido: ");
  Serial.println(is_raining);

  digitalWrite(rain_led, is_raining ? HIGH : LOW);

  String near = check_sensor('m');
  is_near = near.toInt();
  Serial.print("Valor lido: ");
  Serial.println(near);
}

// Atualiza o dashboard com os valores de sensor
void update_dashboard() {
  char * body = (char*) malloc(1024 * 2);

  sprintf(body, "{temperature:%d,luminosity:%d,is_raining:%d,is_near:%d}", temperature, luminosity, is_raining, is_near);

  Serial.print("Request body: ");
  Serial.println(body);
  Serial.print("Enviando dados de sensor para o ThingsBoard..");

  mqttClient.publish(MQTT_TOPIC, body);

  Serial.println(".. pronto!");
  free(body);
}

void loop() {
  int ms = millis();

  const int button_state_read = digitalRead(button_pin);
  Serial.print("Botao: ");
  Serial.println(button_state_read);
  if (button_state_read != previous_button_state) {
    last_button_press = ms;
  }

  if (ms - last_button_press > debounce_delay) {
    if (button_state_read != button_state) {
      button_state = button_state_read;

      if (button_state == HIGH) {
        Serial.println("Botao ligado!");
      } else {
        Serial.println("Botao desligado!");
      }
    }
  }

  if (button_state_read) {
    headlight_override = !headlight_override;
  }

  if (ms - last_updated_sensors >= sensor_update_interval) {
    update_sensors();

    last_updated_sensors = ms;
  }

  if (ms - last_updated_mqtt >= mqtt_update_interval) {
    if (!mqttClient.connected()) {
      ConnectToMqtt();
    }

    update_dashboard();
    last_updated_mqtt = ms;
  }

  mqttClient.loop();
}
