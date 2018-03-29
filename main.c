const int sensorPin = A0;
const int pirPin = 2;

void setup() {
  Serial.begin(9600);

  pinMode(pirPin, INPUT); 
}

void loop() {
  int sensorVal = analogRead(sensorPin);
  int presence = digitalRead(pirPin);

  float voltage = (sensorVal / 1024.0) * 5.0;
  float temperature = (voltage - .5) * 100;
  
  Serial.print("<");
  Serial.print(temperature);
  Serial.print("|");
  Serial.print(presence);
  Serial.print(">");

  delay(1000);
}

