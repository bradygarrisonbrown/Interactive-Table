#include <AccelStepper.h>
// Define step constants
#define FULLSTEP 4
#define HALFSTEP 8

#define motorPin1  8     // Blue   - 28BYJ48 pin 1
#define motorPin2  9     // Grey   - 28BYJ48 pin 3
#define motorPin3  10    // Purple - 28BYJ48 pin 2
#define motorPin4  11    // Green - 28BYJ48 pin 4

// Define stepper interface type
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// Motion settings
const float maxSpeed      = 2000.0;  // steps per second
const float acceleration  = 1000.0;   // steps per second^2

// Target position
long targetPosition = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("AccelStepper Serial Control Ready.");
  Serial.println("Type a target position (e.g. 1000 or -5000).");

  stepper1.setMaxSpeed(maxSpeed);
  stepper1.setAcceleration(acceleration);

  //if (enablePin >= 0) {
  //  pinMode(enablePin, OUTPUT);
  //  digitalWrite(enablePin, LOW);  // LOW = enable for most drivers
  //}
}

void loop() {
  // Check for new serial input
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      long newPos = input.toInt();

      //targetPosition += newPos;  // incremental move
      
      targetPosition = newPos; // Or use absolute positioning:

      Serial.print("New move command: ");
      Serial.print(newPos);
      Serial.print(" steps.  Target position = ");
      Serial.println(targetPosition);

      stepper1.moveTo(targetPosition);
    }
    
  }

  // Run the stepper continuously
  stepper1.run();
}
