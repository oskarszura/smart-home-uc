#include "utils.h"
#include "music.h"

#define TRANSMIT_INTERVAL 1000
unsigned long currentTime;
unsigned long lastTransmit = 0;
unsigned long transmitInterval;
unsigned long lastSample = 0;
unsigned long sampleInterval;

// microphone variables
int sampleMax = 0;
int sampleMin = 1024;
double sampleDb;
const int micPin = A1;

// thermometer variables
const int thermometerPin = A0;

// motion variables
bool isMotion = false;
const int pirPin = 2;

// gas variables
bool isGas = false;
const int gasPin = 4;

// piezzo variables
const int piezzoPin = 7;

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT); 
}

float getSound() {
  int sensorValue = analogRead(micPin);
  return sensorValue;
}

float getTemperature() {
  int sensorValue = analogRead(thermometerPin);
  float voltage = analogToVoltage(sensorValue);
  float temperature = (voltage - .5) * 100;

  return temperature;
}

int getMotion() {
  int isMotion = digitalRead(pirPin);
  return isMotion;
}

int getGas() {
  int isGas = digitalRead(gasPin);
  return isGas;
}

void handleIncomingMsg(int incomingBytes) {
  char cmd[6];
  
  for (int i = 0; i < incomingBytes; i++) {
    cmd[i] = Serial.read(); 
  }

  delay(500);
  if (strcmp(cmd, "CMD001") == 0) {
    playMusic(piezzoPin);
  }
}

void printDataPackage(float temperature, bool motion, bool gas, float sound) {
  Serial.print("<");
  Serial.print(temperature);
  Serial.print("|");
  Serial.print(motion); 
  Serial.print("|");
  Serial.print(gas); 
  Serial.print("|");
  Serial.print(sound); 
  Serial.print(">");  
}

void loop() {
  currentTime = millis();
  transmitInterval = currentTime - lastTransmit;
  sampleInterval = currentTime - lastSample;
  
  float sound = getSound();
  float temperature = getTemperature();
  
  int motion = getMotion();

  if (motion == HIGH) {
    isMotion = true;
  }
  
  int gas = getGas();

  if (gas == LOW) {
    riseAlert(piezzoPin);
    isGas = true;
  }

  if (sampleInterval <= 250UL)
  {
    if (sound > sampleMax)
    {
      sampleMax = sound;
    } 
    else if (sound < sampleMax) {
      sampleMin = sound;
    }
  } else { 
    sampleDb = 20 * log10(analogToVoltage(sampleMax) / analogToVoltage(sampleMin));
    lastSample = currentTime;
  }

  int incomingBytes = Serial.available();

  if (incomingBytes >= 6) {
    handleIncomingMsg(incomingBytes);
  } 

  if (transmitInterval >= 1000UL) {
    lastTransmit = currentTime;
    printDataPackage(temperature, isMotion, isGas, sampleDb); 

    isGas = false;
    isMotion = false;
  }
}
