// thermometer variables
const int thermometerPin = A0;

// motion variables
const int pirPin = 2;
const int motionSpan = 3;
int motionCounter = 0;
int motionState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT); 
}

float getTemperature() {
  int sensorValue = analogRead(thermometerPin);
  float voltage = (sensorValue / 1024.0) * 5.0;
  float temperature = (voltage - .5) * 100;

  return temperature;
}

int getMotion() {
  int isMotion = digitalRead(pirPin);

  if (isMotion == HIGH || motionCounter > 0) {
    if(motionCounter == 0) {
      motionState = millis();
    }
    
    motionCounter += 1;
  }

  if(motionCounter > 0) {
    if(motionCounter > motionSpan) {
      motionCounter = 0;
    }
  } else {
    motionState = 0;
  }

  return motionState;
}

void printDataPackage(float temperature, int motion) {
  Serial.print("<");
  Serial.print(temperature);
  Serial.print("|");
  Serial.print(motion); 
  Serial.print(">");  
}

void loop() {
  float temperature = getTemperature();
  int motion = getMotion();
  
  printDataPackage(temperature, motion);

  delay(1000);
}

