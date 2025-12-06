#ifndef GPIO_STEPPER_MANAGER
#define GPIO_STEPPER_MANAGER

#include <array>

#include "shared.h"

// Not idiomatic RAII but Arduino merges all .ino files together so this is OK
class GpioStepperManager {
  static constexpr float MAX_SPEED = 2000.0;
  static constexpr float ACCELERATION = 200.0;

public:
  // Chose a static factory + private ctor to put all pin knowledge in gpio_stepper_manager.ino
  static GpioStepperManager build();
  void initialize() {
    for (int y = 0; y < Constants::HEIGHT; ++y) {
      for (int x = 0; x < Constants::WIDTH; ++x) {
        steppers_[y][x]->setMaxSpeed(MAX_SPEED);
        steppers_[y][x]->setAcceleration(ACCELERATION);
      }
    }
  }

  void setHeight(xy_t pos, long height) {
    if (pos.y < 0 || pos.y >= Constants::HEIGHT || pos.x < 0 || pos.x >= Constants::WIDTH) {
      Serial.println("Passed bad pos to setHeight");
      return;
    }

    steppers_[pos.y][pos.x]->moveTo(height);
  }

  long distanceToGo(xy_t pos) {
    if (pos.y < 0 || pos.y >= Constants::HEIGHT || pos.x < 0 || pos.x >= Constants::WIDTH) {
      Serial.println("Passed bad pos to distanceTogo");
      return 0;
    }

    return steppers_[pos.y][pos.x]->distanceToGo();
  }

  void step() {
    for (int y = 0; y < Constants::HEIGHT; ++y) {
      for (int x = 0; x < Constants::WIDTH; ++x) {
        steppers_[y][x]->run();
      }
    }
  }

private:
  using StepperGrid = std::array<std::array<AccelStepper*, Constants::WIDTH>, Constants::HEIGHT>;
  StepperGrid steppers_;

  GpioStepperManager(StepperGrid steppers) : steppers_(std::move(steppers)) {}
};

#endif
