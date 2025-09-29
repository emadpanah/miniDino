#include "SonarControl.h"
#include <Arduino.h>

SonarControl::SonarControl(int trig, int echo) : trigPin(trig), echoPin(echo) {}

void SonarControl::begin() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float SonarControl::getDistance() {
  // Trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo time
  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout after 30ms (~5m)

  // Calculate distance
  if (duration == 0 || duration > 23200) { // 23200 µs corresponds to ~400 cm
    return -1; // Out of range or no echo
  }

  float distance = duration * 0.0343 / 2;
  return distance;
}