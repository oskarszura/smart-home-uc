// thermometer variables
const int thermometerPin = A0;

// motion variables
const int pirPin = 2;
const int motionSpan = 3;
int motionCounter = 0;
unsigned long motionState = 0;

// gas variables
const int gasPin = 4;

// piezzo variables
const int piezzoPin = 7;

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

int getGas() {
  int isGas = digitalRead(gasPin);
  return isGas;
}

void printDataPackage(float temperature, int motion, int gas) {
  Serial.print("<");
  Serial.print(temperature);
  Serial.print("|");
  Serial.print(motion);
  Serial.print("|");
  Serial.print(gas);
  Serial.print(">");
}

void loop() {
  float temperature = getTemperature();
  int motion = getMotion();
  int gas = getGas();

  if (gas == LOW) {
    tone(piezzoPin, 4000, 20);
  }

  printDataPackage(temperature, motion, gas);

  delay(1000);
}