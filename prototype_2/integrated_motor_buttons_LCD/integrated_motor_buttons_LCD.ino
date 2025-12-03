#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <LiquidCrystal.h>

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//IO Expander
#include <Adafruit_MCP23X17.h>

// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//Serial
#include <SoftwareSerial.h>
SoftwareSerial link(6, 7);

// NeoPixel setup
#define LED_PIN     8
#define LED_COUNT   256
#define BRIGHTNESS  250
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int red = 0, green = 0, blue = 0;

Adafruit_MCP23X17 mcp;

//Button Stuff - NOTE: THESE PINS ARE FOR MCP
#define BLUE_BUTTON  12

#define YELL_BUTTON  11

#define RED_BUTTON  10

#define GREEN_BUTTON  9

#define WHITE_BUTTON  8


void setup() {
  Serial.begin(9600);
  link.begin(9600);
  Serial.println("Starting MCP23X17 Button Test...");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Clears the LCD screen
  lcd.clear();

  // Initialize MCP23X17 over I2C
  
  if (!mcp.begin_I2C()) {
    Serial.println("Error.");
      // while (1);
    }
  
  Serial.println("MCP23X17 initialized.");

  //Button Setup
  mcp.pinMode(BLUE_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(YELL_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(RED_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(GREEN_BUTTON, INPUT_PULLDOWN);
  mcp.pinMode(WHITE_BUTTON, INPUT_PULLDOWN);

  Serial.println("Button pins configured (using internal pull-ups).");
  Serial.println("Press any button...");

   // Neopixel init
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  strip.begin();
  strip.show(); // Turn off all pixels
  strip.setBrightness(BRIGHTNESS);

  Serial.println("SETUP DONE!");
}
// Global variables to track button state for reliable press detection
static byte lastBlue   = LOW;
static byte lastYellow = LOW;
static byte lastRed    = LOW;
static byte lastGreen  = LOW;
static byte lastWhite  = LOW;

void loop() {
  
  // Call the checking function for each button.
  checkButton(BLUE_BUTTON,  &lastBlue,   0, 0, 150, "0", "BLUE", "BLUE_BUTTON");
  checkButton(YELL_BUTTON,  &lastYellow, 150, 150, 0, "5000", "YELLOW", "YELL_BUTTON");
  checkButton(RED_BUTTON,   &lastRed,    150, 0, 0, "10000", "RED", "RED_BUTTON");
  checkButton(GREEN_BUTTON, &lastGreen,  0, 150, 0, "15000", "GREEN", "GREEN_BUTTON");
  checkButton(WHITE_BUTTON, &lastWhite,  150, 150, 150, "20000", "WHITE", "WHITE_BUTTON");
}

void checkButton(uint8_t pin, byte *lastState, int r, int g, int b, const char* target, const char* msg, const char* buttonName) {
  
  byte currentState = mcp.digitalRead(pin);
  
  // If the state has changed since the last loop iteration
  if (currentState != *lastState) {
    
    
    // Wait for a short debounce period to ensure stability
    delay(20);
    
    // Read the state again after debouncing
    byte debouncedState = mcp.digitalRead(pin);

    // If the state is LOW (pressed) after debouncing, it's a confirmed press
    if (debouncedState == LOW) {
      
      
      buttonUpdateColor(r, g, b, target, msg);
  
      
    } 

  // Update the last state for the next loop iteration
  *lastState = currentState;
}
}

void updateLEDs(){
      // Fill all LEDs with chosen color
    strip.clear();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(red,green,blue));
    }
    strip.show();  // Only call once, after filling all pixels
}

void buttonUpdateColor(int newRed, int newGreen, int newBlue, const char* target, const char* msg ) {
  link.println(target);
  lcd.clear();
  lcd.print(msg);
  red = newRed;
  green = newGreen;
  blue = newBlue;
  updateLEDs();
}
