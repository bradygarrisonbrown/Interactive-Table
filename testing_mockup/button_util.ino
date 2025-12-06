#include "button_util.h"

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