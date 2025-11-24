#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

// =======================================================
// MCP23X17 INSTANCE
// =======================================================
Adafruit_MCP23X17 mcp;

// =======================================================
// STEP SEQUENCE FOR HALF-STEP (8 steps)
// =======================================================
const uint8_t seq[8] = {0x9,0x8,0xC,0x4,0x6,0x2,0x3,0x1};

// =======================================================
// CUSTOM STEPPER CLASS THAT OUTPUTS TO MCP
// Old AccelStepper versions override `step(uint8_t)`
// =======================================================
class MCPStepper : public AccelStepper {
public:
  MCPStepper(uint8_t port, uint8_t offset)
    : AccelStepper(AccelStepper::HALF4WIRE), _port(port), _offset(offset) {}

protected:
  // OLD VERSIONS OF ACCELSTEPPER USE THIS SIGNATURE:
  virtual void step(uint8_t step) {
    uint8_t idx = step & 0x07;
    uint8_t val = seq[idx];

    if (_port == 0) {
      uint8_t cur = mcp.readGPIOA();
      cur &= ~(0x0F << _offset);
      cur |= (val << _offset);
      mcp.writeGPIOA(cur);
    } else {
      uint8_t cur = mcp.readGPIOB();
      cur &= ~(0x0F << _offset);
      cur |= (val << _offset);
      mcp.writeGPIOB(cur);
    }
  }

private:
  uint8_t _port;   // 0 = A, 1 = B
  uint8_t _offset; // nibble offset
};

// =======================================================
// CREATE STEPPERS
// =======================================================
MCPStepper stepper1(1, 0); // B0-B3
MCPStepper stepper2(1, 4); // B4-B7
MCPStepper stepper3(0, 0); // A0-A3

// =======================================================
// MULTISTEPPER
// =======================================================
MultiStepper multi;
long targets[3] = {0,0,0};

// =======================================================
// SERIAL STATE
// =======================================================
int selectedStepper = -1;
bool waitingForStepper = true;
bool waitingForPosition = false;

void promptStepper() {
  Serial.println("\nSelect stepper (1-3):");
  waitingForStepper = true;
  waitingForPosition = false;
}

void promptPosition() {
  Serial.print("Enter new position for stepper ");
  Serial.print(selectedStepper+1);
  Serial.println(":");
  waitingForPosition = true;
  waitingForStepper = false;
}

// =======================================================
// JOG FUNCTION
// =======================================================
void jog(int st, long amount) {
  targets[st] += amount;
  multi.moveTo(targets);
}

// =======================================================
// SETUP
// =======================================================
void setup() {
  Serial.begin(9600);
  delay(200);

  mcp.begin_I2C();
  mcp.writeGPIOA(0);
  mcp.writeGPIOB(0);

  for (int i=0;i<16;i++) mcp.pinMode(i, OUTPUT);

  stepper1.setMaxSpeed(800);
  stepper2.setMaxSpeed(800);
  stepper3.setMaxSpeed(800);

  multi.addStepper(stepper1);
  multi.addStepper(stepper2);
  multi.addStepper(stepper3);

  promptStepper();
}

// =======================================================
// LOOP
// =======================================================
void loop() {

  if (Serial.available()) {
    String in = Serial.readStringUntil('\n');
    in.trim();
    if (!in.length()) return;

    // Jog commands
    if (in.startsWith("+") || in.startsWith("-")) {
      long delta = in.toInt();
      jog(selectedStepper, delta);
      Serial.print("Jogging stepper ");
      Serial.print(selectedStepper+1);
      Serial.print(" by ");
      Serial.println(delta);
      return;
    }

    // Stepper selection
    if (waitingForStepper) {
      int s = in.toInt();
      if (s<1 || s>3) {
        Serial.println("Invalid stepper. Choose 1, 2, or 3.");
        promptStepper();
        return;
      }
      selectedStepper = s-1;
      promptPosition();
      return;
    }

    // Set absolute position
    if (waitingForPosition) {
      long pos = in.toInt();
      targets[selectedStepper] = pos;

      Serial.print("Moving stepper ");
      Serial.print(selectedStepper+1);
      Serial.print(" to ");
      Serial.println(pos);

      multi.moveTo(targets);

      waitingForPosition = false;
      promptStepper();
      return;
    }
  }

  multi.run();
}

