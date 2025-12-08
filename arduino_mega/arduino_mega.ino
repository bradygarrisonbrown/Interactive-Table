#include "gpio_stepper_manager.h"

GpioStepperManager gpioStepperManager = GpioStepperManager::build();

void setup() {
  Serial.begin(9600);

  // put your setup code here, to run once:
  gpioStepperManager.initialize();
  gpioStepperManager.printCalibrationWelcome();
}

bool inCalibration = true;

void loop() {
  if (inCalibration) {
    const auto calibrationDone = gpioStepperManager.calibrationStep();
    if (calibrationDone) {
      Serial.println("###################### CALIBRATION COMPLETE ######################");
      inCalibration = false;
    }
    return;
  }
}
