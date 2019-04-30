#include "music.h"
#include "utils.h"
#include "version.h"
#include <SoftwareSerial.h>

#define TRANSMIT_INTERVAL 1000
unsigned long currentTime;
unsigned long lastTransmit = 0;
unsigned long transmitInterval;
unsigned long lastSample = 0;
unsigned long sampleInterval;

SoftwareSerial softSerial(5, 6); // RX, TX

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
  softSerial.begin(9600);

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
  String cmd = softSerial.readString();

  if (cmd == "CMD001") {
    playMusic(piezzoPin);
  } else if (cmd == "CMDWHO") {
    softSerial.print("[1:agent-" + String(VERSION) + "]");
  } else if (strcmp(cmd, "CMDDIS") == 0) {
    softSerial.print("[0:disconnect]");
  }
}

void printDataPackage(float temperature, bool motion, bool gas, float sound) {
  softSerial.print("[2:<");
  softSerial.print(temperature);
  softSerial.print("|");
  softSerial.print(motion);
  softSerial.print("|");
  softSerial.print(gas);
  softSerial.print("|");
  softSerial.print(sound);
  softSerial.print(">]");
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

  if (sampleInterval <= 250UL) {
    if (sound > sampleMax) {
      sampleMax = sound;
    } else if (sound < sampleMax) {
      sampleMin = sound;
    }
  } else {
    sampleDb =
        20 * log10(analogToVoltage(sampleMax) / analogToVoltage(sampleMin));
    lastSample = currentTime;
  }

  int incomingBytes = softSerial.available();

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
