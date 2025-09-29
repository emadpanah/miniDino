#ifndef SONAR_CONTROL_H
#define SONAR_CONTROL_H

class SonarControl {
public:
  SonarControl(int trigPin, int echoPin); // Constructor with Trig and Echo pins
  void begin();
  float getDistance(); // Returns distance in cm

private:
  int trigPin; // Store the trigger pin
  int echoPin; // Store the echo pin
};

#endif