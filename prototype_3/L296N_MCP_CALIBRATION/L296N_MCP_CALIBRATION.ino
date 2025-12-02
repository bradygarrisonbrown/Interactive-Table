#include <Wire.h>
#include <Adafruit_MCP23X17.h>

// =============================
// MCP EXPANDERS
// =============================
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

// =============================
// MOTOR PIN MAPPING
// =============================
struct MotorPins {
  int in1;
  int in2;
  int en;    
  bool onMCP;
  Adafruit_MCP23X17* mcp;
};

MotorPins motors[9] = {
  {0, 1,  9, true,  &mcp1},   // Motor 1
  {2, 3, 10, true,  &mcp1},   // Motor 2
  {4, 5, 11, true,  &mcp1},   // Motor 3
  {6, 7,  6, true,  &mcp1},   // Motor 4

  {0, 1,  5, true,  &mcp2},   // Motor 5
  {2, 3,  3, true,  &mcp2},   // Motor 6
  {4, 5,  4, true,  &mcp2},   // Motor 7
  {6, 7,  2, true,  &mcp2},   // Motor 8

  {8, 9,  7, false, nullptr}, // Motor 9 (Arduino pins)
};

// =============================
// STATE — time-based position
// =============================
float motorPosition[9] = {0};  // in seconds

// Selection
int selectedMotor = -1;
bool waitingForMotor = true;
bool waitingForTarget = false;

// =============================
// UTILITY FUNCTIONS
// =============================
void setPin(MotorPins &m, int pin, int val) {
  if (m.onMCP) m.mcp->digitalWrite(pin, val);
  else digitalWrite(pin, val);
}

void runMotor(MotorPins &m, int speed, bool forward) {
  if (forward) {
    setPin(m, m.in1, HIGH);
    setPin(m, m.in2, LOW);
  } else {
    setPin(m, m.in1, LOW);
    setPin(m, m.in2, HIGH);
  }
  analogWrite(m.en, speed);
}

void stopMotor(MotorPins &m) {
  setPin(m, m.in1, LOW);
  setPin(m, m.in2, LOW);
  analogWrite(m.en, 0);
}

void promptMotor() {
  Serial.println("\nSelect motor (1–9):");
  waitingForMotor = true;
  waitingForTarget = false;
}

void promptTarget() {
  Serial.print("Enter target absolute time-position (seconds) for motor ");
  Serial.print(selectedMotor + 1);
  Serial.print(". Current = ");
  Serial.println(motorPosition[selectedMotor]);
  waitingForTarget = true;
  waitingForMotor = false;
}

// =============================
// SETUP
// =============================
void setup() {
  Serial.begin(9600);
  delay(200);

  mcp1.begin_I2C();
  mcp2.begin_I2C();

  for (int i = 0; i < 9; i++) {
    MotorPins &m = motors[i];

    if (m.onMCP) {
      m.mcp->pinMode(m.in1, OUTPUT);
      m.mcp->pinMode(m.in2, OUTPUT);
    } else {
      pinMode(m.in1, OUTPUT);
      pinMode(m.in2, OUTPUT);
    }
    pinMode(m.en, OUTPUT);

    stopMotor(m);
  }

  promptMotor();
}

// =============================
// LOOP
// =============================
void loop() {
  if (!Serial.available()) return;

  String in = Serial.readStringUntil('\n');
  in.trim();
  if (!in.length()) return;

  // MOTOR SELECTION
  if (waitingForMotor) {
    int m = in.toInt();
    if (m < 1 || m > 9) {
      Serial.println("Invalid motor.");
      promptMotor();
      return;
    }
    selectedMotor = m - 1;
    promptTarget();
    return;
  }

  // TARGET POSITION ENTRY
  if (waitingForTarget) {
    float target = in.toFloat();
    float current = motorPosition[selectedMotor];

    float diff = target - current;     // positive = forward, negative = reverse
    float duration = abs(diff);        // seconds to move
    bool direction = diff > 0;

    MotorPins &m = motors[selectedMotor];

    Serial.print("Moving motor ");
    Serial.print(selectedMotor + 1);
    Serial.print(" from ");
    Serial.print(current);
    Serial.print(" → ");
    Serial.print(target);
    Serial.print(" (");
    Serial.print(duration);
    Serial.println(" seconds)");

    // Run motor
    if (duration > 0) {
      runMotor(m, 200, direction);
      delay((unsigned long)(duration * 1000));
      stopMotor(m);
    }

    // Update stored absolute position
    motorPosition[selectedMotor] = target;

    Serial.println("Done.");
    promptMotor();
  }
}

