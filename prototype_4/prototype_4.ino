#include <AccelStepper.h>
#include <Adafruit_MCP23X17.h>

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

constexpr int pin1 = 8;
constexpr int pin2 = 9;
constexpr int pin3 = 10;
constexpr int pin4 = 11;

constexpr int step2_pin1 = 12;
constexpr int step2_pin2 = 13;
constexpr int step2_pin3 = 14;
constexpr int step2_pin4 = 15;

constexpr int STEP3_PIN1 = 4;
constexpr int STEP3_PIN2 = STEP3_PIN1 + 1;
constexpr int STEP3_PIN3 = STEP3_PIN1 + 2;
constexpr int STEP3_PIN4 = STEP3_PIN1 + 3;

Adafruit_MCP23X17 mcp1;
AccelStepper arduinoStepper(AccelStepper::FULL4WIRE, pin1, pin3, pin2, pin4);

/* ===== KEEP THESE IN SYNC! ===== */
constexpr int NUM_STEPPERS = 3;
MCPStepper *steppers[3];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("in setup");

  if (!mcp1.begin_I2C()) {
    Serial.println("Error");
  }

  // Help I2C keep up
  Wire.setClock(400000);

  // (!!) Add new steppers here
  steppers[0] = new MCPStepper(&mcp1, pin1, pin3, pin2, pin4);
  steppers[1] = new MCPStepper(&mcp1, step2_pin1, step2_pin3, step2_pin2, step2_pin4);
  steppers[2] = new MCPStepper(&mcp1, STEP3_PIN1, STEP3_PIN3, STEP3_PIN2, STEP3_PIN4);

  for (int i = 0; i < NUM_STEPPERS; ++i) {
    steppers[i]->setMaxSpeed(4000);
    steppers[i]->setAcceleration(4000);
  }

  arduinoStepper.setMaxSpeed(4000);
  arduinoStepper.setAcceleration(4000);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int i = 0; i < NUM_STEPPERS; ++i) {
    steppers[i]->moveTo(10000);
  }
  arduinoStepper.moveTo(10000);

  for (int i = 0; i < NUM_STEPPERS; ++i) {
    steppers[i]->run();
  }
  arduinoStepper.run();
}
