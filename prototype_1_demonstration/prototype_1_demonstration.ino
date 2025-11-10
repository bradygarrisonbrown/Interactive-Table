#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//IO Expander
#include <Adafruit_MCP23X17.h>


// Stepper setup
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

// Motion settings
const float maxSpeed      = 1000.0;
const float acceleration  = 500.0;

long targetPosition = 0;

// NeoPixel setup
#define LED_PIN     6
#define LED_COUNT   256
#define BRIGHTNESS  100
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int red = 0, green = 0, blue = 0;

// Adafruit_MCP23X08 mcp;
Adafruit_MCP23X17 mcp;


//LCD Stuff
// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Button Stuff
#define BLUE_BUTTON  8
byte blueLastButtonState = LOW;

#define YELL_BUTTON  9
byte yellLastButtonState = LOW;

#define RED_BUTTON  10
byte redLastButtonState = LOW;

#define GREEN_BUTTON  11
byte greenLastButtonState = LOW;

#define WHITE_BUTTON  12
byte whiteLastButtonState = LOW;


void setup() {

  //Button Setup
  mcp.pinMode(BLUE_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(YELL_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(RED_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(GREEN_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(WHITE_BUTTON, INPUT_PULLDOWN);



  //Stepper Setup
  Serial.begin(115200);
  Serial.println("AccelStepper Serial Control Ready.");
  Serial.println("Type a target position (e.g. 1000 or -5000).");

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
  strip.show(); // Turn off all pixels
  strip.setBrightness(BRIGHTNESS);
  
}

void loop() {
  //Button Checks

  //Serial.println("LOOP!");

  byte buttonState = mcp.digitalRead(BLUE_BUTTON);
  if (buttonState != blueLastButtonState) {
    blueLastButtonState = buttonState;
    if (buttonState == HIGH) {
      // do an action, for example print on Serial
      buttonUpdateColor(0, 0, 150, 5000, "BLUE!!!");
    }
  }

  buttonState = mcp.digitalRead(YELL_BUTTON);
  if (buttonState != yellLastButtonState) {
    yellLastButtonState = buttonState;
    if (buttonState == HIGH) {
      // do an action, for example print on Serial
      buttonUpdateColor(150, 150, 0, 10000, "YELLOW!!!");
    }
  }

  buttonState = mcp.digitalRead(RED_BUTTON);
  if (buttonState != redLastButtonState) {
    redLastButtonState = buttonState;
    if (buttonState == HIGH) {
      // do an action, for example print on Serial
      buttonUpdateColor(150, 0, 0, 15000, "RED!!!");
    }
  }

  buttonState = mcp.digitalRead(GREEN_BUTTON);
  if (buttonState != greenLastButtonState) {
    greenLastButtonState = buttonState;
    if (buttonState == HIGH) {
      // do an action, for example print on Serial
      buttonUpdateColor(0, 150,  0, 20000, "GREEN!!!");
    }
  }

  buttonState = mcp.digitalRead(WHITE_BUTTON);
  if (buttonState != whiteLastButtonState) {
    whiteLastButtonState = buttonState;
    if (buttonState == HIGH) {
      // do an action, for example print on Serial
      buttonUpdateColor(150, 150, 150, 25000, "WHITE!!!");
    }
  }


  // Check for serial input
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      long newPos = input.toInt();
      targetPosition = newPos;
      updateMotorTargets();
    }

  }

  // Continuously drive the stepper
  multi.run();
}

void updateMotorTargets(){
    Serial.print("New move command: ");
    Serial.println(targetPosition);

    long positions[2];
    positions[0] = targetPosition;     // Stepper A
    positions[1] = targetPosition;    // Stepper B 

    // Command both motors to move
    multi.moveTo(positions);
    //multi.runSpeedToPosition(); // Blocks until done
}


void updateLEDs(){
      // Fill all LEDs with chosen color
    strip.clear();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(blue,green,red));
    }
    strip.show();  // Only call once, after filling all pixels
}

void buttonUpdateColor(int newRed, int newGreen, int newBlue, int position, char* msg ) {
  Serial.println(msg);
  red = newRed;
  green = newGreen;
  blue = newBlue;
  targetPosition = position;
  updateMotorTargets();
  updateLEDs();
}
