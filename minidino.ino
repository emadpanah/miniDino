#include "ServoControl.h"
#include "DisplayControl.h"
#include "SonarControl.h"
#include "MPUControl.h"

// Leg pin setup
const int FRONT_LEFT_PIN  = 6;
const int FRONT_RIGHT_PIN = 9;
const int REAR_LEFT_PIN   = 3;
const int REAR_RIGHT_PIN  = 5;

// Sonar pin setup
const int SONAR_TRIG_PIN = 12;
const int SONAR_ECHO_PIN = 13;

// Distance and tilt thresholds
const float DISTANCE_THRESHOLD = 20.0;
const float WALK_TRIGGER_DISTANCE = 30.0;
const float STABLE_TILT_X_MIN = -5.0;  // بازه بزرگ‌تر برای کاهش حساسیت
const float STABLE_TILT_X_MAX = 5.0;
const float STABLE_TILT_Y_MIN = -8.0;
const float STABLE_TILT_Y_MAX = 8.0;

ServoControl servoController;
DisplayControl screen;
SonarControl sonar(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
MPUControl mpu;

bool isSuperAggressive = false;
bool hasWalked = false;
bool isHappy = false;
bool isSpecialMovement = false;
bool walkingMode = false;
unsigned long happyStartTime = 0;
String currentStatus = "automate"; // حالت پیش‌فرض

void setup() {
  Serial.begin(9600);
  Serial.println(F("MiniDino Setup - Default Status: automate"));

  screen.begin();
  sonar.begin();
  mpu.begin();
  servoController.attachLegs(FRONT_LEFT_PIN, FRONT_RIGHT_PIN, REAR_LEFT_PIN, REAR_RIGHT_PIN);

  servoController.moveAll(90);
  screen.testDot();
  delay(2000);
  screen.drawEyes(HAPPY, false);
}

void loop() {
  static unsigned long lastCheck = 0;
  static float tiltX = 0, tiltY = 0;
  static float distance = -1;

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // حذف فاصله‌های اضافی
    if (command == "status:automate") {
      currentStatus = "automate";
      Serial.println("Status changed to automate");
      servoController.moveAll(90);
    } else if (command == "status:handy") {
      currentStatus = "handy";
      Serial.println("Status changed to handy");
      servoController.moveAll(90);
      isHappy = false; // ریست حالت خوشحال در حالت دستی
    } else if (currentStatus == "handy") {
      if (command == "0") {
        isHappy = true;
        happyStartTime = millis();
        servoController.moveAll(90);
        screen.drawEyes(HAPPY, false);
      } else if (command == "forwardbend") {
        isSpecialMovement = true;
        servoController.forwardBend();
        isSpecialMovement = false;
      } else if (command == "backwardbend") {
        isSpecialMovement = true;
        servoController.backwardBend();
        isSpecialMovement = false;
      } else if (command == "pushup") {
        isSpecialMovement = true;
        servoController.pushUp();
        isSpecialMovement = false;
      } else if (command == "dance") {
        isSpecialMovement = true;
        servoController.dance();
        isSpecialMovement = false;
      } else if (command == "forwardbackwardbend") {
        isSpecialMovement = true;
        servoController.forwardBackwardBend();
        isSpecialMovement = false;
      } else if (command == "frontleftshake") {
        isSpecialMovement = true;
        servoController.frontLeftShake();
        isSpecialMovement = false;
      } else if (command == "frontrightshake") {
        isSpecialMovement = true;
        servoController.frontRightShake();
        isSpecialMovement = false;
      } else if (command == "rearleftshake") {
        isSpecialMovement = true;
        servoController.rearLeftShake();
        isSpecialMovement = false;
      } else if (command == "rearrightshake") {
        isSpecialMovement = true;
        servoController.rearRightShake();
        isSpecialMovement = false;
      } else if (command.length() == 1) {
        char cmd = command.charAt(0);
        if (cmd == 'f') {
          walkingMode = true;
          Serial.println("شروع حرکت اتومات...");
        } else if (cmd == '0') {
          walkingMode = false;
          servoController.moveAll(90);
          Serial.println("توقف کامل.");
        } else {
          servoController.handleManualControl(cmd);
        }
      }
    }
  }

  if (isHappy && (millis() - happyStartTime >= 30000)) {
    isHappy = false;  // بعد از 30 ثانیه به حالت عادی برمی‌گرده
  }

  if (currentStatus == "automate" && !isHappy && !isSpecialMovement) {
    if (millis() - lastCheck > 500) {
      distance = sonar.getDistance();
      tiltX = mpu.getTiltX();
      tiltY = mpu.getTiltY();

      bool isTiltUnstable = (tiltX < STABLE_TILT_X_MIN || tiltX > STABLE_TILT_X_MAX) ||
                            (tiltY < STABLE_TILT_Y_MIN || tiltY > STABLE_TILT_Y_MAX);

      // 🚶 Trigger walk once if object is near
      if (distance > 0 && distance < WALK_TRIGGER_DISTANCE && !hasWalked) {
        screen.drawEyes(SUPER_AGGRESSIVE, false);
        servoController.walkSteps(10);
        hasWalked = true;
      } else if (distance > WALK_TRIGGER_DISTANCE) {
        hasWalked = false; // Reset flag so it can walk again next time
      }

      // 🤖 Eye & tilt reactions
      if (distance > 0 && distance < DISTANCE_THRESHOLD) {
        screen.drawEyes(HAPPY, true, distance, tiltX, tiltY);
        isSuperAggressive = false;
      } else if (isTiltUnstable) {
        screen.drawEyes(SUPER_AGGRESSIVE, false);
        isSuperAggressive = true;
      } else {
        screen.drawEyes(HAPPY, false);
        isSuperAggressive = false;
        servoController.moveAll(90); // neutral stance
      }

      lastCheck = millis();
    }
  }

  // 🦾 If unstable, wiggle legs
  if (isSuperAggressive && !isHappy && !isSpecialMovement && currentStatus == "automate") {
    servoController.runLegs();
  }

  // Run walking mode if active
  if (walkingMode && !isHappy && !isSpecialMovement && currentStatus == "handy") {
    servoController.movePreReq();
    servoController.walkStep();
  }
}