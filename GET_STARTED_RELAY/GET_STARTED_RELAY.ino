int relay = 2;

void setup() {
  // Pin for relay module set as output
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(relay, HIGH);
  delay(3000);
  digitalWrite(relay, LOW);
  delay(3000);
}
