#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *MQTT_SERVER = "broker.hivemq.com";
const char *MQTT_TOPIC = "/telemetry";
const uint16_t MQTT_PORT = 1883;

const char* ssid = "miniIoT";
const char* password = "12345678";

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
  String retorno;

  Serial2.write(sensor);

  // Espera o Arduino responder
  while (!Serial2.available()) {}

  retorno = Serial2.readString();

  Serial.println(retorno);

  return retorno;
}

void ConnectToMqtt() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
    char clientId[100] = "\0";
    sprintf(clientId, "ESP32Client-%04X", random(0xffff));
    Serial.print("Conectando ao MQTT, clientID: ");
    Serial.println(clientId);
    if (mqttClient.connect(clientId)) {
      Serial.println("Conectando ao broker MQTT");
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Atualize os valores de sensor com o Arduino
void update_sensors() {
  String temp = check_sensor('t');
  temperature = temp.toInt();

  String lumens = check_sensor('l');
  luminosity = lumens.toInt();

  String rain = check_sensor('r');
  is_raining = rain.toInt();

  String near = check_sensor('m');
  is_near = near.toInt();
}

// Atualiza o dashboard com os valores de sensor
void update_dashboard() {
  char * body = (char*) malloc(1024 * 2);

  sprintf(body, "{temperature:%d,luminosity:%d,is_raining:%d,is_near:%d}", temperature, luminosity, is_raining, is_near);

  Serial.print("Enviando dados de sensor para o ThingsBoard..");

  mqttClient.publish(MQTT_TOPIC, body);

  Serial.println(".. pronto!");
}

void loop() {
  int ms = millis();

  if (ms - last_updated_sensors >= sensor_update_interval) {
    // update_sensors();
    update_dashboard();
  }

  if (ms - last_updated_mqtt >= mqtt_update_interval) {
    if (!mqttClient.connected()) {
      ConnectToMqtt();
    }
    mqttClient.loop();
  }


  delay(1500);
}
