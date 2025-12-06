#include "output_util.h"

#include <array>

constexpr int LED_PIN = 8;
constexpr int LED_COUNT = 256;
constexpr int LED_BRIGHTNESS = 5;


// MCP 1
constexpr StepperConfig STEP_0 = {4, 5, 6, 7};
constexpr StepperConfig STEP_1 = {8, 9, 10, 11};
constexpr StepperConfig STEP_2 = {12, 13, 14, 15};

// MCP 2
constexpr StepperConfig STEP_3 = {4, 5, 6, 7};
constexpr StepperConfig STEP_4 = {8, 9, 10, 11};
constexpr StepperConfig STEP_5 = {12, 13, 14, 15};

// MCP 3
constexpr StepperConfig STEP_6 = {4, 5, 6, 7};
constexpr StepperConfig STEP_7 = {8, 9, 10, 11};
constexpr StepperConfig STEP_8 = {12, 13, 14, 15};

int xyToIndex(xy_t pos) {
  return pos.y * Constants::HEIGHT + pos.x;
}

void StepperManager::initialize(Adafruit_MCP23X17 *mcp1, Adafruit_MCP23X17 *mcp2, Adafruit_MCP23X17 *mcp3) {
#ifdef TESTING
  return;
#else
  steppers_[0] = new MCPStepper(mcp1, STEP_0.p1, STEP_0.p3, STEP_0.p2, STEP_0.p4);
  steppers_[1] = new MCPStepper(mcp1, STEP_1.p1, STEP_1.p3, STEP_1.p2, STEP_1.p4);
  steppers_[2] = new MCPStepper(mcp1, STEP_2.p1, STEP_2.p3, STEP_2.p2, STEP_2.p4);

  steppers_[3] = new MCPStepper(mcp2, STEP_3.p1, STEP_3.p3, STEP_3.p2, STEP_3.p4);
  steppers_[4] = new MCPStepper(mcp2, STEP_4.p1, STEP_4.p3, STEP_4.p2, STEP_4.p4);
  steppers_[5] = new MCPStepper(mcp2, STEP_5.p1, STEP_5.p3, STEP_5.p2, STEP_5.p4);

  steppers_[6] = new MCPStepper(mcp3, STEP_6.p1, STEP_6.p3, STEP_6.p2, STEP_6.p4);
  steppers_[7] = new MCPStepper(mcp3, STEP_7.p1, STEP_7.p3, STEP_7.p2, STEP_7.p4);
  steppers_[8] = new MCPStepper(mcp3, STEP_8.p1, STEP_8.p3, STEP_8.p2, STEP_8.p4);

  for (int i = 0; i < steppers_.size(); ++i) {
    steppers_[i]->setMaxSpeed(4000);
    steppers_[i]->setAcceleration(4000);
  }
#endif
}

void StepperManager::setHeight(xy_t pos, long height) {
#ifdef TESTING
  return;
#else
  if (pos.x < 0 || pos.x >= Constants::WIDTH || pos.y < 0 || pos.y >= Constants::HEIGHT) {
    return;
  }

  steppers_[xyToIndex(pos)]->moveTo(height);
#endif
}

long StepperManager::distanceToGo(xy_t pos) {
#ifdef TESTING
  return 0;
#else
  if (pos.x < 0 || pos.x >= Constants::WIDTH || pos.y < 0 || pos.y >= Constants::HEIGHT) {
    return 0;
  }

  // TODO: REMOVE SILLY HACK WHEN WE FIX MOTOR 8
  if (pos.x == 2 && pos.y == 2) {
    return 0;
  }

  return steppers_[xyToIndex(pos)]->distanceToGo();
#endif
}

void StepperManager::step() {
#ifdef TESTING
  return;
#else
  for (int i = 0; i < steppers_.size(); ++i) {
    steppers_[i]->run();
  }
#endif
}



#ifndef TESTING
xy_t randomMole() {
  return { random(0, 3), random(0, 3) };
}
#else
xy_t randomMole() {
  return { 1, 1 };
}
#endif
