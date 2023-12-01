#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

WiFiUDP udp;
Coap coap(udp);

const char* ssid = "miniIoT";
const char* password = "12345678";

int last_updated_sensors = 0;
int sensor_update_interval = 1000;

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

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  Serial.print("Recebi resposta: ");
  Serial.println(p);
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

  // Define a função na qual recebemos respostas do servidor
  coap.response(callback_response);

  coap.start();
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
  String body = "{";
  body += "temperature:200";
  body += "}";

  Serial.println("Envie esta bomba pro ThingsBoard");
  coap.put(IPAddress(44, 196, 172, 187), 5683, "api/v1/mbxQvYcGHTeTAFJBsZZH/telemetry", body.c_str());
  
  Serial.println("Envie esta bomba pro ThingsBoard");
}

void loop() {
  int ms = millis();

  if (ms - last_updated_sensors >= sensor_update_interval) {
    // update_sensors();
    update_dashboard();
  }

  coap.loop();

  delay(1500);
}
