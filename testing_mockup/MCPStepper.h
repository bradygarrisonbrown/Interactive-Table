#ifndef MCP_STEPPER_H
#define MCP_STEPPER_H

#include <AccelStepper.h>
#include <Adafruit_MCP23X17.h>

struct StepperConfig {
  uint8_t p1, p2, p3, p4;
};

/**
 * Copy/paste this class where ever you want to use stuff
 */
class MCPStepper : public AccelStepper {
  Adafruit_MCP23X17 *mcp_;
  uint8_t pin_[4];

public:
  MCPStepper(Adafruit_MCP23X17 *mcp, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4): AccelStepper(AccelStepper::FULL4WIRE, p1, p2, p3, p4), mcp_(mcp) {
    pin_[0] = p1;
    pin_[1] = p2;
    pin_[2]= p3;
    pin_[3] = p4;

    enableOutputs();
  }

  void enableOutputs() override {
    mcp_->pinMode(pin_[0], OUTPUT);
    mcp_->pinMode(pin_[1], OUTPUT);
    mcp_->pinMode(pin_[2], OUTPUT);
    mcp_->pinMode(pin_[3], OUTPUT);
  }

protected:
  void setOutputPins(uint8_t mask) override {
    const uint8_t numpins = 4;
    for (int i = 0; i < numpins; ++i) {
      // PRAY that it's never inverted :shrug:
      // mcp_->digitalWrite(pin_[i], (mask & (1 << i)) ? (HIGH ^ _pinInverted[i]) : (LOW ^ _pinInverted[i]));
      mcp_->digitalWrite(pin_[i], (mask & (1 << i)) ? (HIGH) : (LOW));
    }
  }
};

#endif // MCP_STEPPER_H