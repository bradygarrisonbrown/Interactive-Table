#include <AccelStepper.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Stepper setup
#define HALFSTEP 8
#define motorPin1  8
#define motorPin2  9
#define motorPin3  10
#define motorPin4  11

AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// NeoPixel setup
#define LED_PIN     6
#define LED_COUNT   256
#define BRIGHTNESS  50

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Motion settings
const float maxSpeed      = 2000.0;
const float acceleration  = 1000.0;

long targetPosition = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("AccelStepper Serial Control Ready.");
  Serial.println("Type a target position (e.g. 1000 or -5000).");

  stepper1.setMaxSpeed(maxSpeed);
  stepper1.setAcceleration(acceleration);

  // Neopixel init
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  strip.begin();
  strip.show(); // Turn off all pixels
  strip.setBrightness(BRIGHTNESS);
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      long newPos = input.toInt();
      targetPosition = newPos;

      Serial.print("New move command: ");
      Serial.println(targetPosition);

      stepper1.moveTo(targetPosition);
    }

    // --- Set color based on target position ---
    int red = 0, green = 0, blue = 0;

    if (targetPosition > 5000) {
      red = 150;
    } else if (targetPosition >= 3000) {
      green = 150;
    } else {
      blue = 150;
    }

    // Fill all LEDs with chosen color
    strip.clear();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(red, green, blue));
    }
    strip.show();  // Only call once, after filling all pixels
  }

  // Continuously drive the stepper
  stepper1.run();
}

