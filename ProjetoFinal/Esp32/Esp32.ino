#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *MQTT_SERVER = "mqtt.thingsboard.cloud";
const char *MQTT_TOPIC = "v1/devices/me/telemetry";
const char *MQTT_CLIENT_ID = "wk4ult0143kbgyuwvlgo";
const char *MQTT_USERNAME = "cyj5dkus0rnsgjl9bl6x";
const char *MQTT_PASSWORD = "of8jh40a96tc96q2832s";
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

String check_sensor(char sensor) {
  if(sensor == 'r') {
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
    Serial.println(MQTT_CLIENT_ID);
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Conectado ao broker MQTT");
      mqttClient.subscribe(MQTT_TOPIC);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  //  /pinMode(led_interno, OUTPUT);
  // / pinMode(buzzer, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

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

  String rain = check_sensor('r');
  is_raining = rain.toInt();
  Serial.print("Valor lido: ");
  Serial.println(is_raining);

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
}

void loop() {
  int ms = millis();

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
