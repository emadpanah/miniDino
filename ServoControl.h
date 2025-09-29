#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Servo.h>

enum LegPosition {
  FRONT_LEFT,
  FRONT_RIGHT,
  REAR_LEFT,
  REAR_RIGHT
};

class ServoControl {
public:
  ServoControl();
  void attachLegs(int fl, int fr, int rl, int rr);
  void moveLeg(LegPosition leg, int angle);
  void moveAll(int angle);
  void walkSteps(int stepCount);
  void runLegs();
  void movePreReq();
  void walkStep();
  void forwardBend();
  void backwardBend();
  void pushUp();
  void dance();
  void forwardBackwardBend();
  void frontLeftShake();
  void frontRightShake();
  void rearLeftShake();
  void rearRightShake();
  void handleManualControl(char cmd);

private:
  Servo legs[4];
  int mirror[4] = {0, 0, 1, 1}; // REAR legs are mirrored
};

#endif