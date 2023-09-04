const int ledPin = LED_BUILTIN;
const int buzzerPin = 2;

int ledState = LOW;
int buzzerState = LOW;

int State = 0;


unsigned long previousMillis = 0;

const long interval = 100000;


void setup() {
  // put your setup code here, to run once:

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  Serial.println(State);

}

void loop() {
  Serial.print("State: ");
  Serial.println(State);
  
  if (Serial.available() > 0) {
    // Read a byte and ignore it
    Serial.read();
    State = 0;
  }

  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    State++;
    State = State % 3;

    Serial.println(State);

    switch (State) {
      case 0:
        buzzerState = LOW;
        ledState = LOW;

        break;

      case 1:
        ledState = HIGH;
        buzzerState = LOW;
        break;

      default:
        ledState = HIGH;
        buzzerState = HIGH;
        break;
    }

    digitalWrite(buzzerPin, buzzerState);
    digitalWrite(ledPin, ledState);
  }
}