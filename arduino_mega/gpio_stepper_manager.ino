
#include "gpio_stepper_manager.h"
#include <AccelStepper.h>

struct GpioStepperConfig {
  uint8_t p1, p2, p3, p4;
};

constexpr GpioStepperConfig GPIO_STEPPER_0_CONFIG = {2, 3, 4, 5};
constexpr GpioStepperConfig GPIO_STEPPER_1_CONFIG = {6, 7, 8, 9};
constexpr GpioStepperConfig GPIO_STEPPER_2_CONFIG = {22, 24, 26, 28};
constexpr GpioStepperConfig GPIO_STEPPER_3_CONFIG = {23, 25, 27, 29};
constexpr GpioStepperConfig GPIO_STEPPER_4_CONFIG = {30, 32, 34, 36};
constexpr GpioStepperConfig GPIO_STEPPER_5_CONFIG = {31, 33, 35, 37};
constexpr GpioStepperConfig GPIO_STEPPER_6_CONFIG = {38, 40, 42, 44};
constexpr GpioStepperConfig GPIO_STEPPER_7_CONFIG = {39, 41, 43, 45};
constexpr GpioStepperConfig GPIO_STEPPER_8_CONFIG = {46, 48, 50, 52};

#define DEFINE_STEPPER(ID) \
    AccelStepper GPIO_STEPPER_##ID( \
        AccelStepper::HALF4WIRE, \
        GPIO_STEPPER_##ID##_CONFIG.p1, \
        GPIO_STEPPER_##ID##_CONFIG.p3, \
        GPIO_STEPPER_##ID##_CONFIG.p2, \
        GPIO_STEPPER_##ID##_CONFIG.p4 \
    )

DEFINE_STEPPER(0);
DEFINE_STEPPER(1);
DEFINE_STEPPER(2);
DEFINE_STEPPER(3);
DEFINE_STEPPER(4);
DEFINE_STEPPER(5);
DEFINE_STEPPER(6);
DEFINE_STEPPER(7);
DEFINE_STEPPER(8);

GpioStepperManager GpioStepperManager::build() {
  static StepperGrid stepperGrid = {
    {&GPIO_STEPPER_0, &GPIO_STEPPER_1, &GPIO_STEPPER_2},
    {&GPIO_STEPPER_3, &GPIO_STEPPER_4, &GPIO_STEPPER_5},
    {&GPIO_STEPPER_6, &GPIO_STEPPER_7, &GPIO_STEPPER_8}
  };
  return GpioStepperManager{stepperGrid};
}

