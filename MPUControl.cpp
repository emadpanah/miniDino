#include "MPUControl.h"

MPUControl::MPUControl() {}

void MPUControl::begin() {
  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

float MPUControl::getTiltX() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // Tilt X = atan2(accel_y, sqrt(accel_x^2 + accel_z^2)) * 180/PI
  float tiltX = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
  return tiltX;
}

float MPUControl::getTiltY() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // Tilt Y = atan2(-accel_x, sqrt(accel_y^2 + accel_z^2)) * 180/PI
  float tiltY = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
  return tiltY;
}