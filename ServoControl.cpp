#include "ServoControl.h"
#include <Arduino.h>

ServoControl::ServoControl() {}

void ServoControl::attachLegs(int fl, int fr, int rl, int rr) {
  legs[FRONT_LEFT].attach(fl);
  legs[FRONT_RIGHT].attach(fr);
  legs[REAR_LEFT].attach(rl);
  legs[REAR_RIGHT].attach(rr);
}

void ServoControl::moveLeg(LegPosition leg, int angle) {
  int actualAngle = mirror[leg] ? (180 - angle) : angle;
  legs[leg].write(actualAngle);
}

void ServoControl::moveAll(int angle) {
  for (int i = 0; i < 4; i++) {
    moveLeg((LegPosition)i, angle);
  }
}

void ServoControl::runLegs() {
  static bool up = true;
  static int angle = 90;

  if (up) {
    angle += 5;
    if (angle >= 100) up = false;
  } else {
    angle -= 5;
    if (angle <= 80) up = true;
  }

  moveAll(angle);
  delay(50);
}

void ServoControl::walkSteps(int stepCount) {
  for (int i = 0; i < stepCount; i++) {
    moveLeg(FRONT_RIGHT, 100);
    moveLeg(REAR_LEFT, 80);
    delay(300);
    moveLeg(FRONT_RIGHT, 90);
    moveLeg(REAR_LEFT, 90);
    delay(300);
    moveLeg(FRONT_LEFT, 100);
    moveLeg(REAR_RIGHT, 80);
    delay(300);
    moveLeg(FRONT_LEFT, 90);
    moveLeg(REAR_RIGHT, 90);
    delay(300);
  }
  moveAll(90);
}

void ServoControl::movePreReq() {
  moveLeg(FRONT_RIGHT, 90);
  moveLeg(FRONT_LEFT, 90);
  moveLeg(REAR_LEFT, 125);
  moveLeg(REAR_RIGHT, 55);
  delay(100);
}

void ServoControl::walkStep() {
  // مرحله 1: پاهای جلو x/2 جلو
  moveLeg(FRONT_RIGHT, 115);
  moveLeg(FRONT_LEFT, 65);
  delay(250);

  // مرحله 2: پاهای عقب x جلو
  moveLeg(REAR_LEFT, 70);
  moveLeg(REAR_RIGHT, 110);
  delay(250);

  // مرحله 3: بازگشت به حالت اولیه
  movePreReq();
  delay(300);
}

void ServoControl::forwardBend() {
  // پاهای جلو به آرامی از 90 به 10 و 170 برن و برگردن، 3 بار
  for (int j = 0; j < 3; j++) {
    for (int angle = 90; angle >= 10; angle -= 1) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle); // FRL به 170
      delay(70); // خیلی آهسته
    }
    delay(300); // نگه داشتن در حالت خم
    for (int angle = 10; angle <= 90; angle += 1) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(70);
    }
  }
}

void ServoControl::backwardBend() {
  // پاهای عقب به آرامی از 90 به 170 و 10 برن و برگردن، 3 بار
  for (int j = 0; j < 3; j++) {
    for (int angle = 90; angle <= 170; angle += 1) {
      moveLeg(REAR_LEFT, 180 - angle); // RLL به 170
      moveLeg(REAR_RIGHT, angle); // RRL به 10
      delay(50); // خیلی آهسته
    }
    delay(300); // نگه داشتن در حالت خم
    for (int angle = 170; angle >= 90; angle -= 1) {
      moveLeg(REAR_LEFT, 180 - angle);
      moveLeg(REAR_RIGHT, angle);
      delay(50);
    }
  }
}

void ServoControl::pushUp() {
  // مثل forward bend ولی 3 بار و سریع‌تر
  for (int i = 0; i < 3; i++) {
    for (int angle = 90; angle >= 10; angle -= 1) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(50); // سریع‌تر
    }
    for (int angle = 10; angle <= 90; angle += 1) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(100);
    }
  }
}

void ServoControl::dance() {
  // ترکیبی از چهار حرکت قبلی برای 3 بار، با اضافه کردن shake بعد از هر حرکت
  for (int j = 0; j < 2; j++) {
    // forward bend سریع
    for (int angle = 90; angle >= 10; angle -= 5) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(15);
    }
    for (int angle = 10; angle <= 90; angle += 5) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(15);
    }
    this->frontRightShake(); // shake بعد از forward bend

    // backward bend سریع
    for (int angle = 90; angle <= 170; angle += 5) {
      moveLeg(REAR_LEFT, 180 - angle);
      moveLeg(REAR_RIGHT, angle);
      delay(15);
    }
    for (int angle = 170; angle >= 90; angle -= 5) {
      moveLeg(REAR_LEFT, 180 - angle);
      moveLeg(REAR_RIGHT, angle);
      delay(15);
    }
    this->rearLeftShake(); // shake بعد از backward bend

    // pushup سریع
    for (int angle = 90; angle >= 10; angle -= 5) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(15);
    }
    for (int angle = 10; angle <= 90; angle += 5) {
      moveLeg(FRONT_LEFT, angle);
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(15);
    }
    this->frontLeftShake(); // shake بعد از pushup

    // forward backward bend
    this->forwardBackwardBend();
    this->rearRightShake(); // shake بعد از forward backward bend
  }
}

void ServoControl::forwardBackwardBend() {
  // ترکیب خم شدن جلو و عقب با تأخیر آهسته، 3 بار، همزمان
  for (int j = 0; j < 3; j++) {
    // حرکت همزمان از 90 به حداقل/حداکثر
    for (int angle = 90; angle >= 10; angle -= 1) {
      moveLeg(FRONT_LEFT, angle);         // FLL به 10
      moveLeg(FRONT_RIGHT, 180 - angle);  // FRL به 170
      moveLeg(REAR_LEFT, 180 - (90 + (90 - angle))); // RLL به 170
      moveLeg(REAR_RIGHT, 90 + (90 - angle));       // RRL به 10
      delay(25); // آهسته
    }
    delay(300); // نگه داشتن در حالت خم
    // بازگشت همزمان به 90
    for (int angle = 10; angle <= 90; angle += 1) {
      moveLeg(FRONT_LEFT, angle);         // FLL به 90
      moveLeg(FRONT_RIGHT, 180 - angle);  // FRL به 90
      moveLeg(REAR_LEFT, 180 - (90 + (90 - angle))); // RLL به 90
      moveLeg(REAR_RIGHT, 90 + (90 - angle));       // RRL به 90
      delay(25);
    }
  }
}

void ServoControl::frontLeftShake() {
  // خم شدن جداگانه FLL، 5 بار با تأخیر سریع
  for (int i = 0; i < 5; i++) {
    for (int angle = 90; angle >= 10; angle -= 5) {
      moveLeg(FRONT_LEFT, angle);
      delay(10); // سریع
    }
    for (int angle = 10; angle <= 90; angle += 5) {
      moveLeg(FRONT_LEFT, angle);
      delay(10);
    }
  }
}

void ServoControl::frontRightShake() {
  // خم شدن جداگانه FRL، 5 بار با تأخیر سریع
  for (int i = 0; i < 5; i++) {
    for (int angle = 90; angle >= 10; angle -= 5) {
      moveLeg(FRONT_RIGHT, 180 - angle); // معکوس
      delay(10); // سریع
    }
    for (int angle = 10; angle <= 90; angle += 5) {
      moveLeg(FRONT_RIGHT, 180 - angle);
      delay(10);
    }
  }
}

void ServoControl::rearLeftShake() {
  // خم شدن جداگانه RLL، 5 بار با تأخیر سریع
  for (int i = 0; i < 5; i++) {
    for (int angle = 90; angle >= 10; angle -= 5) {
      moveLeg(REAR_LEFT, angle); // معکوس
      delay(10); // سریع
    }
    for (int angle = 10; angle <= 90; angle += 5) {
      moveLeg(REAR_LEFT, angle);
      delay(10);
    }
  }
}

void ServoControl::rearRightShake() {
  // خم شدن جداگانه RRL، 5 بار با تأخیر سریع
  for (int i = 0; i < 5; i++) {
    for (int angle = 90; angle >= 10; angle -= 5) {
      moveLeg(REAR_RIGHT, 180 - angle);
      delay(10); // سریع
    }
    for (int angle = 10; angle <= 90; angle += 5) {
      moveLeg(REAR_RIGHT, 180 - angle);
      delay(10);
    }
  }
}

void ServoControl::handleManualControl(char cmd) {
  switch (cmd) {
    case 'o': // FRL باز
      moveLeg(FRONT_RIGHT, legs[FRONT_RIGHT].read() + 5);
      break;
    case 'p': // FRL بسته
      moveLeg(FRONT_RIGHT, legs[FRONT_RIGHT].read() - 5);
      break;
    case 'q': // FLL باز
      moveLeg(FRONT_LEFT, legs[FRONT_LEFT].read() + 5);
      break;
    case 'w': // FLL بسته
      moveLeg(FRONT_LEFT, legs[FRONT_LEFT].read() - 5);
      break;
    case 'a': // RLL باز
      moveLeg(REAR_LEFT, legs[REAR_LEFT].read() + 5);
      break;
    case 's': // RLL بسته
      moveLeg(REAR_LEFT, legs[REAR_LEFT].read() - 5);
      break;
    case 'k': // RRL باز
      moveLeg(REAR_RIGHT, legs[REAR_RIGHT].read() + 5);
      break;
    case 'l': // RRL بسته
      moveLeg(REAR_RIGHT, legs[REAR_RIGHT].read() - 5);
      break;
  }
}