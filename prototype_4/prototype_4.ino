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

    Serial.println("initialized");
    enableOutputs();
  }

  void enableOutputs() override {
    Serial.println("outputs enabled!");
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


Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;
Adafruit_MCP23X17 mcp3;

/* ===== KEEP THESE IN SYNC! ===== */
constexpr int NUM_STEPPERS = 9;
std::array<MCPStepper*, NUM_STEPPERS> steppers;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("in setup");

  if (!mcp1.begin_I2C()) {
    Serial.println("Error starting mcp1");
  }

  if (!mcp2.begin_I2C(MCP23XXX_ADDR + 1)) {
    Serial.println("Error starting mcp2");
  }

  if (!mcp3.begin_I2C(MCP23XXX_ADDR + 2)) {
    Serial.println("Error starting mcp3");
  }

  // Help I2C keep up (I got this from asking LLM, says it should be safe)
  Wire.setClock(400000);

  // (!!) Add new steppers here
  steppers[0] = new MCPStepper(&mcp1, STEP_0.p1, STEP_0.p3, STEP_0.p2, STEP_0.p4);
  steppers[1] = new MCPStepper(&mcp1, STEP_1.p1, STEP_1.p3, STEP_1.p2, STEP_1.p4);
  steppers[2] = new MCPStepper(&mcp1, STEP_2.p1, STEP_2.p3, STEP_2.p2, STEP_2.p4);

  steppers[3] = new MCPStepper(&mcp2, STEP_3.p1, STEP_3.p3, STEP_3.p2, STEP_3.p4);
  steppers[4] = new MCPStepper(&mcp2, STEP_4.p1, STEP_4.p3, STEP_4.p2, STEP_4.p4);
  steppers[5] = new MCPStepper(&mcp2, STEP_5.p1, STEP_5.p3, STEP_5.p2, STEP_5.p4);

  steppers[6] = new MCPStepper(&mcp3, STEP_6.p1, STEP_6.p3, STEP_6.p2, STEP_6.p4);
  steppers[7] = new MCPStepper(&mcp3, STEP_7.p1, STEP_7.p3, STEP_7.p2, STEP_7.p4);
  steppers[8] = new MCPStepper(&mcp3, STEP_8.p1, STEP_8.p3, STEP_8.p2, STEP_8.p4);

  for (int i = 0; i < steppers.size(); ++i) {
    steppers[i]->setMaxSpeed(4000);
    steppers[i]->setAcceleration(4000);
  }
}

void loop() {
  for (int i = 0; i < steppers.size(); ++i) {
    steppers[i]->moveTo(10000);
  }

  for (int i = 0; i < steppers.size(); ++i) {
    steppers[i]->run();
  }
}
