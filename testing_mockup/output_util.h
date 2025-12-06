#pragma once

#include "shared.h"
#include "MCPStepper.h"
#include <array>


class StepperManager {
  std::array<MCPStepper*, Constants::HEIGHT * Constants::WIDTH> steppers_;
public:
  void initialize(Adafruit_MCP23X17 *mcp1, Adafruit_MCP23X17 *mcp2, Adafruit_MCP23X17 *mcp3);
  void setHeight(xy_t pos, long height);
  long distanceToGo(xy_t pos);
  void step();
};



xy_t randomMole();

ButtonGrid readButtons();