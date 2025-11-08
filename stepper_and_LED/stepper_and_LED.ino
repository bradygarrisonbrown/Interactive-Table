#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

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

// NeoPixel setup
#define LED_PIN     6
#define LED_COUNT   256
#define BRIGHTNESS  5

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Motion settings
const float maxSpeed      = 1000.0;
const float acceleration  = 500.0;

long targetPosition = 0;

void setup() {
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
  // Check for serial input
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      long newPos = input.toInt();
      targetPosition = newPos;

      Serial.print("New move command: ");
      Serial.println(targetPosition);

      long positions[2];
      positions[0] = targetPosition;     // Stepper A
      positions[1] = targetPosition;    // Stepper B 

      // Command both motors to move
      multi.moveTo(positions);
      //multi.runSpeedToPosition(); // Blocks until done
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
  multi.run();
}

