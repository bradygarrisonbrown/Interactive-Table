#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//IO Expander
#include <Adafruit_MCP23X17.h>

#include <SoftwareSerial.h>
SoftwareSerial link(6, 7);


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


void setup() {

  //Stepper Setup
  Serial.begin(9600);
  link.begin(9600);
  Serial.println("AccelStepper Serial Control Ready.");
  Serial.println("Type a target position (e.g. 1000 or -5000).");

  stepperA.setMaxSpeed(maxSpeed);
  stepperA.setAcceleration(acceleration);
  stepperB.setMaxSpeed(maxSpeed);
  stepperB.setAcceleration(acceleration);

  multi.addStepper(stepperA);
  multi.addStepper(stepperB);
  
}

void loop() {
  //Button Checks

  // Check for serial input
  if (link.available() > 0) {
    String input = link.readStringUntil('\n');
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

  }

  // Continuously drive the stepper
  multi.run();
}

