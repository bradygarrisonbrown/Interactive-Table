// Tutorial: https://learn.adafruit.com/adafruit-mcp23017-i2c-gpio-expander/arduino

// Blinks an LED attached to a MCP23XXX pin.

// ok to include only the one needed
// both included here to make things simple for example
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>

#define LED_PIN 0     // MCP23XXX pin LED is attached to, I/O expander frame of reference.
// FOR DEMO: use pin 0 on each I/O Expander

// uncomment appropriate line
// Adafruit_MCP23X08 mcp;
constexpr uint8_t MCP_COUNT = 2;
Adafruit_MCP23X17 mcp[MCP_COUNT];

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  Serial.println("MCP23xxx Blink Test!");

  for (int i = 0; i < MCP_COUNT; ++i) {
    // IMPORTANT: You need to assign a unique i2c addr to each i/o expander!!!
    // Correspondingly: You must set D0, D1, D2 to the right offset for it work.
    if (!mcp[i].begin_I2C(MCP23XXX_ADDR + i)) {
      Serial.println("Error.");
      // while (1);
    }
    mcp[i].pinMode(LED_PIN, OUTPUT);
  }

  Serial.println("Looping...");
}

void loop() {
  // Always keep 2nd mcp LED ON
  mcp[1].digitalWrite(LED_PIN, HIGH);

  mcp[0].digitalWrite(LED_PIN, HIGH);
  delay(500);
  mcp[0].digitalWrite(LED_PIN, LOW);
  delay(500);
}