#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <MCPStepper.h>

// Create two MCP expanders
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

// Create steppers on different MCPs
MCPStepper stepperA(&mcp1, 0, 1, 2, 3, FULL_STEP);
MCPStepper stepperB(&mcp1, 4, 5, 6, 7, HALF_STEP);

MCPStepper stepperC(&mcp2, 0, 1, 2, 3, HALF_STEP);

void setup() {
  Wire.begin();

  mcp1.begin_I2C(0x20);
  mcp2.begin_I2C(0x21);

  stepperA.setSpeed(12);  // RPM
  stepperB.setSpeed(8);
  stepperC.setSpeed(15);
}

void loop() {
  // Example: non-blocking multi-stepper control
  stepperA.step(2048);  // 1 rev
  stepperB.step(4096);  // 1 rev half-step
  stepperC.step(4096);

  while (true) {
    bool aDone = stepperA.run();
    bool bDone = stepperB.run();
    bool cDone = stepperC.run();

    if (aDone && bDone && cDone)
      break;
  }

  delay(1000);
}
