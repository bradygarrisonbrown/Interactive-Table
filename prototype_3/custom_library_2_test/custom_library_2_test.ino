#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <MCPAccelStepper.h>

// Two expanders
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

// 8 steppers on MCPs, 1 on Arduino pins
MCPAccelStepper step1(&mcp1, 0, 1, 2, 3);
MCPAccelStepper step2(&mcp1, 4, 5, 6, 7);
MCPAccelStepper step3(&mcp1, 8, 9, 10, 11);
MCPAccelStepper step4(&mcp1, 12, 13, 14, 15);

MCPAccelStepper step5(&mcp2, 0, 1, 2, 3);
MCPAccelStepper step6(&mcp2, 4, 5, 6, 7);
MCPAccelStepper step7(&mcp2, 8, 9, 10, 11);
MCPAccelStepper step8(&mcp2, 12, 13, 14, 15);

// Regular stepper on Arduino GPIO
AccelStepper step9(AccelStepper::FULL4WIRE, 4, 5, 6, 7);

MultiStepper multi;

void setup() {
  Wire.begin();

  mcp1.begin_I2C(0x20);
  mcp2.begin_I2C(0x21);

  // Add all 9 steppers to the MultiStepper system
  multi.addStepper(step1);
  multi.addStepper(step2);
  multi.addStepper(step3);
  multi.addStepper(step4);
  multi.addStepper(step5);
  multi.addStepper(step6);
  multi.addStepper(step7);
  multi.addStepper(step8);
  multi.addStepper(step9);

  // Set speed/acceleration
  step1.setMaxSpeed(500);
  step1.setAcceleration(200);
}

void loop() {
  // Example: move every motor to position 1000 steps
  long positions[9] = {1000,1000,1000,1000,1000,1000,1000,1000,1000};
  multi.moveTo(positions);

  // Run all synchronized
  multi.runSpeedToPosition();

  delay(1000);
}

