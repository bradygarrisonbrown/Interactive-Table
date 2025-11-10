#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>   // Include LCD library

#ifdef __AVR__
  #include <avr/power.h>
#endif

// ----- Stepper setup -----
#define HALFSTEP 8

#define motorAPin1  8
#define motorAPin2  9
#define motorAPin3  10
#define motorAPin4  11

#define motorBPin1  2
#define motorBPin2  3
#define motorBPin3  4
#define motorBPin4  5

AccelStepper stepperA(HALFSTEP, motorAPin1, motorAPin3, motorAPin2, motorAPin4);
AccelStepper stepperB(HALFSTEP, motorBPin1, motorBPin3, motorBPin2, motorBPin4);

MultiStepper multi;

// ----- Motion settings -----
const float maxSpeed      = 1000.0;
const float acceleration  = 500.0;
long targetPosition = 0;

// ----- NeoPixel setup -----
#define LED_PIN     6
#define LED_COUNT   256
#define BRIGHTNESS  10
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int red = 0, green = 0, blue = 0;

// ----- Button setup -----
#define RED_BUTTON  7
byte lastButtonState = LOW;

// ----- LCD setup (using pins from tutorial) -----
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // rs, enable, d4, d5, d6, d7

// =====================================================
void setup() {
  pinMode(RED_BUTTON, INPUT);

  Serial.begin(115200);

  // Stepper setup
  stepperA.setMaxSpeed(maxSpeed);
  stepperA.setAcceleration(acceleration);
  stepperB.setMaxSpeed(maxSpeed);
  stepperB.setAcceleration(acceleration);
  multi.addStepper(stepperA);
  multi.addStepper(stepperB);

  // Neopixel init
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);

  // LCD init
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("System Ready");
}

// =====================================================
void loop() {
  byte buttonState = digitalRead(RED_BUTTON);
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    if (buttonState == HIGH) {
      buttonUpdateColor(150, 0, 0, 5000, "RED!!!");
    }
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      long newPos = input.toInt();
      targetPosition = newPos;
      updateMotorTargets();
    }
  }

  multi.run();
}

// =====================================================
void updateMotorTargets() {
  long positions[2];
  positions[0] = targetPosition;
  positions[1] = targetPosition;
  multi.moveTo(positions);
}

// =====================================================
void updateLEDs() {
  strip.clear();
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(blue, green, red));
  }
  strip.show();
}

// =====================================================
void buttonUpdateColor(int newRed, int newGreen, int newBlue, int position, const char* msg) {
  // Update internal values
  red = newRed;
  green = newGreen;
  blue = newBlue;
  targetPosition = position;

  // Update hardware
  updateMotorTargets();
  updateLEDs();

  // ---- LCD OUTPUT (replaces Serial.print) ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Color Change:");
  lcd.setCursor(0, 1);
  lcd.print(msg);
}