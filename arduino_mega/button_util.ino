#include "button_util.h"

//Button Pins
// This is VERY HW dependent and may change if we rewire.
// Watch out!!
constexpr int BUTTON_1 = 54;
constexpr int BUTTON_2 = 55;
constexpr int BUTTON_3 = 56;
constexpr int BUTTON_4 = 57;
constexpr int BUTTON_5 = 58;
constexpr int BUTTON_6 = 59;
constexpr int BUTTON_7 = 60;
constexpr int BUTTON_8 = 61;
constexpr int BUTTON_9 = 62;

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
constexpr int EMERGENCY_STOP = 19;

/*
BUTTON HANDLING CODE
*/

//Emergency stop interrupt function
void emergencyStopISR(){
  emergencyStop = true;
}


//Initialize button + emergency stop pin settings
void initializeButtons() {
  //Button Setup
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  pinMode(BUTTON_5, INPUT_PULLUP);
  pinMode(BUTTON_6, INPUT_PULLUP);
  pinMode(BUTTON_7, INPUT_PULLUP);
  pinMode(BUTTON_8, INPUT_PULLUP);
  pinMode(BUTTON_9, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(EMERGENCY_STOP), emergencyStopISR, RISING);
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
  ButtonGrid grid;

  for (int i = 0; i < 9; i++) {
      bool val = checkButton(buttonIds[i], lasts[i]);
      grid.buttons[i / 3][i % 3] = val;
  }

  return grid;
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

      if (debouncedState == HIGH) {
        ret = true;
      } 

    // Update the last state for the next loop iteration
    *lastState = currentState;
  }
  return ret;
}