#include "DisplayControl.h"

extern bool stopEyeCycle;

// Base time in Tehran (IRST): May 14, 2025, 11:16:00
const unsigned long BASE_SECONDS = 11 * 3600 + 16 * 60; // 11:16:00 in seconds

DisplayControl::DisplayControl() : oled(U8G2_R1, U8X8_PIN_NONE) {}

void DisplayControl::begin() {
  oled.begin();
  oled.setContrast(255);
  oled.setFont(u8g2_font_5x7_tf); // Small font to fit more text
  Serial.println(F("OLED initialized"));
  startTime = millis(); // Record startup time
}

void DisplayControl::testDot() {
  oled.clearBuffer();
  oled.drawPixel(10, 10);
  oled.sendBuffer();
  Serial.println(F("Test dot drawn at (10, 10)"));
}

void DisplayControl::drawHappyEye(int x, int y, bool blink) {
  if (blink) {
    int centerY = y + eyeHeight / 2;
    int curveRadius = 0;
    int lineWidth = eyeWidth - 5;

    for (int dy = -11; dy <= 0; dy++) {
      int startY = centerY + dy;
      int endY = startY;
      oled.drawDisc(x, startY, curveRadius, U8G2_DRAW_ALL);
      oled.drawDisc(x + lineWidth, endY, curveRadius, U8G2_DRAW_ALL);
      oled.drawHLine(x + curveRadius, startY, lineWidth - 2 * curveRadius);
    }

    for (int dy = 6; dy <= 17; dy++) {
      int startY = centerY + dy;
      int endY = startY;
      oled.drawDisc(x, startY, curveRadius, U8G2_DRAW_ALL);
      oled.drawDisc(x + lineWidth, endY, curveRadius, U8G2_DRAW_ALL);
      oled.drawHLine(x + curveRadius, startY, lineWidth - 2 * curveRadius);
    }
  } else {
    int radius = eyeWidth / 2 + 1;
    oled.drawDisc(x + eyeWidth / 2, y + radius, radius, U8G2_DRAW_ALL);
  }
}

void DisplayControl::drawSeriousEye(int x, int y, bool blink) {
  int radius = eyeWidth / 2 - 2;
  if (!blink) {
    oled.drawDisc(x + eyeWidth / 2, y + radius, radius, U8G2_DRAW_ALL);
  }
}

void DisplayControl::drawAggressiveEye(int x, int y) {
  int centerY = y + eyeHeight / 2;
  int curveRadius = 1;
  int lineWidth = eyeWidth - 5;

  for (int dy = -11; dy <= 0; dy++) {
    int startY = centerY + dy;
    int endY = startY;
    oled.drawDisc(x, startY, curveRadius, U8G2_DRAW_ALL);
    oled.drawDisc(x + lineWidth, endY, curveRadius, U8G2_DRAW_ALL);
    oled.drawHLine(x + curveRadius, startY, lineWidth - 2 * curveRadius);
  }
}

void DisplayControl::drawSuperAggressiveEye(int x, int y) {
  int centerY = y + eyeHeight / 2;
  int curveRadius = 1;
  int lineWidth = eyeWidth - 5;

  for (int dy = -11; dy <= 0; dy++) {
    int startX = x;
    int startY = centerY + dy;
    int endX = x + lineWidth;
    int endY = startY - 12;

    if (x == leftEyeX) {
      int tempX = startX;
      int tempY = startY;
      startX = tempX;
      startY = tempY;
      endX = tempX + lineWidth;
      endY = tempY + 12;
    } else {
      startY = centerY + dy + 12;
      endY = startY - 12;
    }

    oled.drawDisc(startX, startY, curveRadius, U8G2_DRAW_ALL);
    oled.drawDisc(endX, endY, curveRadius, U8G2_DRAW_ALL);
    oled.drawLine(startX + curveRadius, startY, endX - curveRadius, endY);
  }
}

void DisplayControl::drawHappyMouth(bool blink) {
  if (blink) {
    int reducedWidth = mouthWidth / 2;
    int centerX = mouthX + mouthWidth / 2 - reducedWidth / 2;
    for (int offset = -1; offset <= 3; offset++) {
      oled.drawHLine(centerX, mouthY + offset - 1, reducedWidth);
    }
  } else {
    int centerX = mouthX + mouthWidth / 2;
    int radius = mouthWidth / 2;
    for (int offset = -1; offset <= 3; offset++) {
      for (int i = 0; i < 180; i += 10) {
        float angle = radians(i);
        int x = centerX + radius * cos(angle);
        int y = mouthY + radius * sin(angle) + offset;
        oled.drawPixel(x, y - 1);
      }
    }
  }
}

void DisplayControl::drawSeriousMouth() {
  for (int offset = -1; offset <= 4; offset++) {
    oled.drawHLine(mouthX, mouthY + offset + 5, mouthWidth);
  }
}

void DisplayControl::drawAggressiveMouth() {
  int centerX = mouthX + mouthWidth / 2 - 1;
  int radius = mouthWidth / 2;
  for (int offset = -1; offset <= 3; offset++) {
    for (int i = 190; i < 360; i += 10) {
      float angle = radians(i);
      int x = centerX + radius * cos(angle);
      int y = mouthY + radius * sin(angle) + offset;
      oled.drawPixel(x, y + 10);
    }
  }
}

void DisplayControl::drawSuperAggressiveMouth() {
  int segments = 5;
  int segmentWidth = mouthWidth / segments;
  int height = 5;
  for (int offset = -1; offset <= 3; offset++) {
    for (int i = 0; i < segments; i++) {
      int x1 = mouthX + i * segmentWidth;
      int x2 = x1 + segmentWidth / 2;
      int x3 = x1 + segmentWidth;
      int y1 = mouthY + offset;
      int y2 = mouthY - height + offset;
      int y3 = mouthY + offset;
      oled.drawLine(x1 - 1, y1, x2, y2);
      oled.drawLine(x2 - 1, y2, x3, y3);
    }
  }
}

void DisplayControl::drawEyes(uint8_t style, bool showLogs, float distance, float tiltX, float tiltY) {
  if (showLogs) {
    // Show MPU and sonar logs with Tehran time (IRST)
    oled.firstPage();
    do {
      oled.setCursor(0, 10);
      oled.print(F("Tilt X: "));
      oled.print(tiltX);
      oled.print(F(" deg"));
      oled.setCursor(0, 20);
      oled.print(F("Tilt Y: "));
      oled.print(tiltY);
      oled.print(F(" deg"));
      oled.setCursor(0, 30);
      oled.print(F("Dist: "));
      oled.print(distance);
      oled.print(F(" cm"));
      oled.setCursor(0, 40);
      unsigned long elapsedTime = (millis() - startTime) / 1000; // Seconds since begin
      unsigned long totalSeconds = BASE_SECONDS + elapsedTime;
      int hours = (totalSeconds / 3600) % 24; // Hours within a day
      int minutes = (totalSeconds % 3600) / 60;
      int seconds = totalSeconds % 60;
      char timeStr[16];
      sprintf(timeStr, "14/05/25 %02d:%02d:%02d", hours, minutes, seconds);
      oled.print(timeStr);
    } while (oled.nextPage());
    Serial.println(F("Displaying MPU and sonar logs with Tehran time"));
  } else {
    // Show eye style
    oled.firstPage();
    do {
      switch (style) {
        case HAPPY:
          drawHappyEye(leftEyeX, eyesY, blinkState);
          drawHappyEye(rightEyeX, eyesY, blinkState);
          drawHappyMouth(blinkState);
          break;
        case SERIOUS:
          drawSeriousEye(leftEyeX, eyesY, blinkState);
          drawSeriousEye(rightEyeX, eyesY, blinkState);
          drawSeriousMouth();
          break;
        case AGGRESSIVE:
          drawAggressiveEye(leftEyeX, eyesY);
          drawAggressiveEye(rightEyeX, eyesY);
          drawAggressiveMouth();
          break;
        case SUPER_AGGRESSIVE:
          drawSuperAggressiveEye(leftEyeX, eyesY);
          drawSuperAggressiveEye(rightEyeX, eyesY);
          drawSuperAggressiveMouth();
          break;
      }
    } while (oled.nextPage());
    Serial.println(F("Displaying eye style"));
  }
}

void DisplayControl::updateBlinkState() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastBlinkTime > 1000) {
    lastBlinkTime = currentMillis;
    blinkState = true;
    delay(200);
    blinkState = false;
  }
}

void DisplayControl::loopEyeStyles(bool enableBlinking) {
  for (int i = 0; i < NUM_STATES; i++) {
    if (stopEyeCycle) {
      Serial.println(F("Eye style cycle interrupted"));
      break;
    }
    if (enableBlinking && (i == HAPPY || i == SERIOUS)) {
      updateBlinkState();
    }
    drawEyes(i, false, -1, 0, 0);
    unsigned long startTime = millis();
    while (millis() - startTime < 3000) {
      if (stopEyeCycle) {
        Serial.println(F("Eye style cycle interrupted during delay"));
        break;
      }
      delay(100);
    }
  }
}

void DisplayControl::drawLogs(float distance, float tiltX, float tiltY) {
  // Integrated into drawEyes, kept for compatibility
  oled.firstPage();
  do {
    oled.setCursor(0, 10);
    oled.print(F("Tilt X: "));
    oled.print(tiltX);
    oled.print(F(" deg"));
    oled.setCursor(0, 20);
    oled.print(F("Tilt Y: "));
    oled.print(tiltY);
    oled.print(F(" deg"));
    oled.setCursor(0, 30);
    oled.print(F("Dist: "));
    oled.print(distance);
    oled.print(F(" cm"));
    oled.setCursor(0, 40);
    unsigned long elapsedTime = (millis() - startTime) / 1000; // Seconds since begin
    unsigned long totalSeconds = BASE_SECONDS + elapsedTime;
    int hours = (totalSeconds / 3600) % 24; // Hours within a day
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    char timeStr[16];
    sprintf(timeStr, "14/05/25 %02d:%02d:%02d", hours, minutes, seconds);
    oled.print(timeStr);
  } while (oled.nextPage());
}