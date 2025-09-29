#ifndef MPU_CONTROL_H
#define MPU_CONTROL_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class MPUControl {
public:
  MPUControl();
  void begin();
  float getTiltX(); // Returns tilt angle in degrees along X-axis
  float getTiltY(); // Returns tilt angle in degrees along Y-axis

private:
  Adafruit_MPU6050 mpu;
};

#endif