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
#define BLUE_BUTTON  8

#define YELL_BUTTON  9

#define RED_BUTTON  10

#define GREEN_BUTTON  11

#define WHITE_BUTTON  12


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
    Serial.println("MCP23X17 not found! Check wiring or address (default 0x20).");
    while (1);
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

void loop() {
  // Buttons are wired to ground, so LOW = pressed
  byte buttonState = mcp.digitalRead(BLUE_BUTTON);
  if (buttonState == HIGH) {
  //Serial.println("BLUE!");
  // do an action, for example print on Serial
  buttonUpdateColor(0, 0, 150, "0", "BLUE");
  }
  

  buttonState = mcp.digitalRead(YELL_BUTTON);
  if (buttonState == HIGH) {
    //Serial.println("YELLOW!");
    // do an action, for example print on Serial
    buttonUpdateColor(150, 150, 0, "5000", "YELLOW");
  }
  

  buttonState = mcp.digitalRead(RED_BUTTON);
  if (buttonState == HIGH) {
    //Serial.println("RED!");
    // do an action, for example print on Serial
    buttonUpdateColor(150, 0, 0, "10000", "RED");
  }
  

  buttonState = mcp.digitalRead(GREEN_BUTTON);
  if (buttonState == HIGH) {
    //Serial.println("GREEN!");
    // do an action, for example print on Serial
    buttonUpdateColor(0, 150,  0, "15000", "GREEN");
  }
  

  buttonState = mcp.digitalRead(WHITE_BUTTON);
  
  if (buttonState == HIGH) {
    //Serial.println("WHITE!");
    // do an action, for example print on Serial
    buttonUpdateColor(150, 150, 150, "20000", "WHITE");
  }

  

  delay(200); // debounce + avoid spamming the serial monitor
}


void updateLEDs(){
      // Fill all LEDs with chosen color
    strip.clear();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(red,green,blue));
    }
    strip.show();  // Only call once, after filling all pixels
}

void buttonUpdateColor(int newRed, int newGreen, int newBlue, char* target, char* msg ) {
  link.println(target);
  lcd.clear();
  lcd.print(msg);
  red = newRed;
  green = newGreen;
  blue = newBlue;
  updateLEDs();
}
