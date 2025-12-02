#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <AccelStepper.h>
#include <MCPStepper.h>
#include <MultiStepper.h>


// Create two MCP expanders
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

// Create steppers on different MCPs
MCPStepper stepper1(&mcp1, 0, 1, 2, 3, FULL_STEP);
MCPStepper stepper2(&mcp1, 4, 5, 6, 7, FULL_STEP);
MCPStepper stepper3(&mcp1, 8, 9, 10, 11, FULL_STEP);
MCPStepper stepper4(&mcp1, 12, 13, 14, 15, FULL_STEP);
MCPStepper stepper5(&mcp2, 0, 1, 2, 3, FULL_STEP);
MCPStepper stepper6(&mcp2, 4, 5, 6, 7, FULL_STEP);
MCPStepper stepper7(&mcp2, 8, 9, 10, 11, FULL_STEP);
MCPStepper stepper8(&mcp2, 12, 13, 14, 15, FULL_STEP);
AccelStepper stepper9(FULL_STEP, 4, 5, 6, 7);

// =======================================================
// MULTISTEPPER
// =======================================================
MultiStepper multi;
long targets[9] = {0,0,0,0,0,0,0,0,0};

// =======================================================
// SERIAL STATE
// =======================================================
int selectedStepper = -1;
bool waitingForStepper = true;
bool waitingForPosition = false;

void promptStepper() {
  Serial.println("\nSelect stepper (1-9):");
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

  mcp1.begin_I2C();
  mcp2.begin_I2C();

  stepper1.setSpeed(800);
  stepper2.setSpeed(800);
  stepper3.setSpeed(800);
  stepper4.setSpeed(800);
  stepper5.setSpeed(800);
  stepper6.setSpeed(800);
  stepper7.setSpeed(800);
  stepper8.setSpeed(800);
  stepper9.setMaxSpeed(800);

  multi.addStepper(stepper1);
  multi.addStepper(stepper2);
  multi.addStepper(stepper3);
  multi.addStepper(stepper4);
  multi.addStepper(stepper5);
  multi.addStepper(stepper6);
  multi.addStepper(stepper7);
  multi.addStepper(stepper8);
  multi.addStepper(stepper9);

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
      if (s<1 || s>9) {
        Serial.println("Invalid stepper. Choose 1-9");
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
