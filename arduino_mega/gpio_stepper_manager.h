#ifndef GPIO_STEPPER_MANAGER
#define GPIO_STEPPER_MANAGER

#include "shared.h"
#include <AccelStepper.h>

// Not idiomatic RAII but Arduino merges all .ino files together so this is OK
class GpioStepperManager {
  static constexpr float MAX_SPEED = 4000.0;
  static constexpr float ACCELERATION = 600.0;
  static constexpr long SMALL_STEP = 4096.0;

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
      // Serial.println("Passed bad pos to distanceTogo");
      return 0;
    }

    // HACK: motor driver at (1, 1) failed, use this to work around that
    if (pos.x == 1 && pos.y == 1) {
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
  
  void printCalibrationWelcome() {
    Serial.println(F("--- STEPPER CALIBRATION TOOL ---"));
    Serial.println(F("Controls:"));
    Serial.println(F(" 'u' = Nudge Up (CW)"));
    Serial.println(F(" 'd' = Nudge Down (CCW)"));
    Serial.println(F(" 'z' = SET ZERO HERE"));
    Serial.println(F("--------------------------------"));
  }

  /**
  Returns true when calibration is COMPLETE
  */
  bool calibrationStep() {
    static int stepperIndex = 0;

    if (stepperIndex == Constants::HEIGHT * Constants::WIDTH) {
      return true;
    }

    const auto stepper = steppers_[stepperIndex / 3][stepperIndex % 3];
    
    if (Serial.available() > 0) {
      const char peeked = Serial.peek();

      if (isDigit(peeked) || peeked == '-' || peeked == '+') {
        // User types something like +/- 100
        long stepsToMove = Serial.parseInt(); 

        Serial.print(F("Calibrating stepper: "));
        Serial.println(stepperIndex);
        
        Serial.print(F("Moving manually: "));
        Serial.println(stepsToMove);
        
        stepper->move(stepsToMove);
        
        // while (Serial.available() > 0 && Serial.read() != '\n');
      } else {
        const auto command = Serial.read();

        switch (command) {
          case 'u':
            Serial.print(F("Calibrating stepper: "));
            Serial.println(stepperIndex + 1);
            Serial.println(F("Nudging CW..."));
            stepper->move(SMALL_STEP); 
            break;

          case 'd':
            Serial.print(F("Calibrating stepper: "));
            Serial.println(stepperIndex + 1);
            Serial.println(F("Nudging CCW..."));
            stepper->move(-SMALL_STEP); 
            break;

          case 'z': // ZERO POSITION
            stepper->setCurrentPosition(0);
            Serial.println(F(">>> ZERO POSITION SET! <<<"));
            Serial.print(F("Current Position is now: "));
            Serial.println(stepper->currentPosition());
            stepperIndex++;
            if (stepperIndex < 9) {
              Serial.print(F("Moving to stepper: "));
              Serial.println(stepperIndex + 1);
            }
            break;
            
          // Ignore newlines and carriage returns
          case '\n':
          case '\r':
          case ' ':
            break;
            
          default:
            // Optional: print help if unknown key pressed
            // Serial.println(F("Unknown key. Use u/d/U/D/z"));
            break;
        }
      }
    }

    // 2. ALWAYS run the stepper
    // This allows the motor to travel to the new target set by .move()
    step();

    return false;
  }

private:
  using StepperGrid = AccelStepper*[Constants::HEIGHT][Constants::WIDTH];
  StepperGrid steppers_;

  GpioStepperManager(StepperGrid steppers) {
    for (int y = 0; y < Constants::HEIGHT; ++y) {
      for (int x = 0; x < Constants::WIDTH; ++x) {
        steppers_[y][x] = steppers[y][x];
      }
    }
  }
};

#endif
