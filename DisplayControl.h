#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include <U8g2lib.h>

enum EyeState {
  HAPPY,
  SERIOUS,
  AGGRESSIVE,
  SUPER_AGGRESSIVE,
  NUM_STATES
};

class DisplayControl {
public:
  DisplayControl();
  void begin();
  void drawEyes(uint8_t style, bool showLogs = false, float distance = -1, float tiltX = 0, float tiltY = 0);
  void loopEyeStyles(bool enableBlinking);
  void updateBlinkState();
  void testDot();

private:
  U8G2_SH1107_128X128_1_HW_I2C oled; // Page buffer mode to save RAM
  void drawHappyEye(int x, int y, bool blink);
  void drawSeriousEye(int x, int y, bool blink);
  void drawAggressiveEye(int x, int y);
  void drawSuperAggressiveEye(int x, int y);
  void drawHappyMouth(bool blink);
  void drawSeriousMouth();
  void drawAggressiveMouth();
  void drawSuperAggressiveMouth();
  void drawLogs(float distance, float tiltX, float tiltY);

  const int eyeWidth = 56;
  const int eyeHeight = 56;
  const int leftEyeX = 2;
  const int rightEyeX = 70;
  const int eyesY = 0;
  const int mouthY = eyesY + eyeHeight + 12;
  const int mouthWidth = 50;
  const int mouthX = (leftEyeX + rightEyeX) / 2 + 5;

  bool blinkState = false;
  unsigned long lastBlinkTime = 0;
  unsigned long startTime; // Track startup time
};

#endif