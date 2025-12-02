#include <Wire.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

// Motor A PWM (from Arduino)
int enA = 9;
// Motor B PWM (from Arduino)
int enB = 3;

// Motor A direction pins (through MCP)
int in1 = 0;   // MCP pin GPA0
int in2 = 1;   // MCP pin GPA1
// Motor B direction pins
int in3 = 2;   // MCP pin GPA2
int in4 = 3;   // MCP pin GPA3

void setup() {
  mcp.begin_I2C();  // default address 0x20

  // MCP direction pins
  mcp.pinMode(in1, OUTPUT);
  mcp.pinMode(in2, OUTPUT);
  mcp.pinMode(in3, OUTPUT);
  mcp.pinMode(in4, OUTPUT);

  // PWM pins on Arduino
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  // Motors off
  mcp.digitalWrite(in1, LOW);
  mcp.digitalWrite(in2, LOW);
  mcp.digitalWrite(in3, LOW);
  mcp.digitalWrite(in4, LOW);
}

void loop() {
  // forward
  mcp.digitalWrite(in1, HIGH);
  mcp.digitalWrite(in2, LOW);
  mcp.digitalWrite(in3, HIGH);
  mcp.digitalWrite(in4, LOW);
  analogWrite(enA, 200);
  analogWrite(enB, 200);
  delay(2000);

  // reverse
  mcp.digitalWrite(in1, LOW);
  mcp.digitalWrite(in2, HIGH);
  mcp.digitalWrite(in3, LOW);
  mcp.digitalWrite(in4, HIGH);
  delay(2000);

  // stop
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  mcp.digitalWrite(in1, LOW);
  mcp.digitalWrite(in2, LOW);
  mcp.digitalWrite(in3, LOW);
  mcp.digitalWrite(in4, LOW);
  delay(2000);
}

