void setup() {
  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  digitalWrite(8,LOW);
  analogWrite(9,0);
  digitalWrite(10,HIGH);
  analogWrite(11,255);
  delay(100);
}