int val = 0;
const int touch_gpio = 3;

// Initialized on `setup`
int cycles_count;

bool got_touched() {
  long new_count = touchRead(touch_gpio);

  bool was_touched = new_count < cycles_count;
  if(was_touched) {
    cycles_count = new_count; 
  }

  return was_touched;
}

void setup() {
  int cycles_count = touchRead(touch_gpio);
  Serial.begin(9600);
}
void loop() {
  val = hallRead();
  Serial.println(val);
  delay(100);
}
