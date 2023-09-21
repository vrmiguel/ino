#define SOUND_SPEED 0.034

const int trigPin = 5;
const int echoPin = 18;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  delay(100);
}

void startSonar() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

// Lê a distância em centímetros
float sonarRead() {
  const long duration = pulseIn(echoPin, HIGH);
  const float distanceCm = duration * SOUND_SPEED / 2;

  return distanceCm;
}

// Média móvel de 5 elementos
float rolling_avg() {
  int i;

  float sum = 0;
  for (i = 0; i < 5; i++) {
    sum += sonarRead();
  }

  return sum / 5;
}

int loopIdx = 0;
float measures[5];

void loop() {
  Serial.println("Capturando info do sonar");
  
  startSonar();
  float read = sonarRead();
  Serial.print("sonarRead: ");
  Serial.println(read);

  if(loopIdx == 5) {
    float sum = 0;
    int i;
    for(i = 0; i < 5; i++) {
      sum += measures[i];
    }

    float avg = sum/5;
    Serial.print("Media movel: ");
    Serial.println(avg);
    loopIdx = 0;
  } else {
    measures[loopIdx++] = read;
  }

  delay(500);
}
