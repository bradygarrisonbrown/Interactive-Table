
#include "gpio_stepper_manager.h"
#include <AccelStepper.h>

struct GpioStepperConfig {
  uint8_t p1, p2, p3, p4;
};

constexpr GpioStepperConfig GPIO_STEPPER_0_CONFIG = {0, 1, 2, 3};
constexpr GpioStepperConfig GPIO_STEPPER_1_CONFIG = {4, 5, 6, 7};
constexpr GpioStepperConfig GPIO_STEPPER_2_CONFIG = {8, 9, 10, 11};
constexpr GpioStepperConfig GPIO_STEPPER_3_CONFIG = {12, 13, 14, 15};
constexpr GpioStepperConfig GPIO_STEPPER_4_CONFIG = {16, 17, 18, 19};
constexpr GpioStepperConfig GPIO_STEPPER_5_CONFIG = {20, 21, 22, 23};
constexpr GpioStepperConfig GPIO_STEPPER_6_CONFIG = {24, 25, 26, 27};
constexpr GpioStepperConfig GPIO_STEPPER_7_CONFIG = {28, 29, 30, 31};
constexpr GpioStepperConfig GPIO_STEPPER_8_CONFIG = {32, 33, 34, 35};

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
  return GpioStepperManager{
    StepperGrid{{
      {&GPIO_STEPPER_0, &GPIO_STEPPER_1, &GPIO_STEPPER_2},
      {&GPIO_STEPPER_3, &GPIO_STEPPER_4, &GPIO_STEPPER_5},
      {&GPIO_STEPPER_6, &GPIO_STEPPER_7, &GPIO_STEPPER_8}
    }}
  };
}

