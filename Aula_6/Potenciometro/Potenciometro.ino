float measures[MEASURE_COUNT];
const int MEASURE_COUNT = 4;
int loopIdx = 0;

float read_volts() {
  int step_value = analogRead(A0);
  float step_value_f = (float) step_value;
 
  const float volts = step_value_f * (5.0 / 1024.0);
  return volts;
}

float read_temperature() {
  // Leia a voltagem convertendo pra mV
  const float v_in = read_volts() * 1000.0;

  // T = 10 * v_in
  return v_in / 10;
}

void setup() {
  pinMode(A0, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  float temp = read_temperature();
  Serial.print("Temperatura lida: ");
  Serial.println(temp);

  delay(100);
}
