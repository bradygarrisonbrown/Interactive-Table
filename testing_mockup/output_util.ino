#include "output_util.h"

#include "Arduino_LED_Matrix.h"
#include <array>

constexpr int LED_PIN = 8;
constexpr int LED_COUNT = 256;
constexpr int LED_BRIGHTNESS = 5;

constexpr uint32_t EMPTY = 0;

//Button Pins
constexpr int BUTTON_1 = 4;
constexpr int BUTTON_2 = 5;
constexpr int BUTTON_3 = 6;
constexpr int BUTTON_4 = 7;
constexpr int BUTTON_5 = 8;
constexpr int BUTTON_6 = 9;
constexpr int BUTTON_7 = 10;
constexpr int BUTTON_8 = 11;
constexpr int BUTTON_9 = 12;

// Global variables to track button state for reliable press detection
static byte last1   = LOW;
static byte last2 = LOW;
static byte last3   = LOW;
static byte last4  = LOW;
static byte last5  = LOW;
static byte last6  = LOW;
static byte last7  = LOW;
static byte last8  = LOW;
static byte last9  = LOW;

//Emergency Stop Pin
constexpr int EMERGENCY_STOP = 3;


ArduinoLEDMatrix ledMatrix;

void initializeLED() {
  ledMatrix.begin();
}

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

  // Serial.println("Set Height:");
  // Serial.println(pos.x);
  // Serial.println(pos.y);
  // Serial.println(height);

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

  // TODO: REMOVE SILLY HACK UNTIL WE FIX MOTOR 8
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

/* ================ LED mock up ================ */
// constexpr int LED_HEIGHT = 12;
// constexpr int LED_WIDTH = 8;
// std::array<std::array<byte, LED_HEIGHT>, LED_WIDTH> frame_{};
// void StepperManager::setHeight(xy_t pos, long height) {
//   if (height == 0) {
//     frame_[pos.y][pos.x] = 0;
//   } else {
//     frame_[pos.y][pos.x] = 1;
//   }
//   ledMatrix.renderBitmap(frame_.data(), LED_HEIGHT, LED_WIDTH);
// }

void displayLCD(const char *msg, int score) {}

#ifndef TESTING
xy_t randomMole() {
  return { random(0, 3), random(0, 3) };
}
#else
xy_t randomMole() {
  return { 1, 1 };
}
#endif


/*
BUTTON HANDLING CODE
*/

//Initialize button + emergency stop pin settings
void initializeButtons() {
  //Button Setup
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  pinMode(BUTTON_2, INPUT_PULLDOWN);
  pinMode(BUTTON_3, INPUT_PULLDOWN);
  pinMode(BUTTON_4, INPUT_PULLDOWN);
  pinMode(BUTTON_5, INPUT_PULLDOWN);
  pinMode(BUTTON_6, INPUT_PULLDOWN);
  pinMode(BUTTON_7, INPUT_PULLDOWN);
  pinMode(BUTTON_8, INPUT_PULLDOWN);
  pinMode(BUTTON_9, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(EMERGENCY_STOP), ISR, RISING);

}

//Emergency stop interrupt function
void ISR(){
  //serial.println("EMERGENCY STOP!")
  while(true){ }
}

//Arrays to keep track of button/Last items
int buttonIds[9] = {
    BUTTON_1, BUTTON_2, BUTTON_3,
    BUTTON_4, BUTTON_5, BUTTON_6,
    BUTTON_7, BUTTON_8, BUTTON_9
};

byte* lasts[9] = {
    &last1, &last2, &last3,
    &last4, &last5, &last6,
    &last7, &last8, &last9
};

//Runs check button on each button and returns 3x3 array of boolean button status
ButtonGrid readButtons() {
  std::array<std::array<bool, 3>, 3> arr;

  for (int i = 0; i < 9; i++) {
      bool val = checkButton(buttonIds[i], lasts[i]);
      arr[i / 3][i % 3] = val;
  }

  return arr;
}

//Returns boolean that is true if button successfully pressed
bool checkButton(uint8_t pin, byte *lastState) {
  bool ret = false;
  byte currentState = digitalRead(pin);
  
  // If the state has changed since the last loop iteration
  if (currentState != *lastState) {
    
    
    // Wait for a short debounce period to ensure stability
    delay(20);
    
    // Read the state again after debouncing
    byte debouncedState = digitalRead(pin);

    // If the state is LOW (pressed) after debouncing, it's a confirmed press
    if (debouncedState == LOW) {
      ret = true;
    } 

  // Update the last state for the next loop iteration
  *lastState = currentState;
  return ret;
}
}