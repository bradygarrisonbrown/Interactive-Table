#include "output_util.h"

#include <Adafruit_NeoPixel.h>

constexpr int LED_PIN = 8;
constexpr int LED_COUNT = 256;
constexpr int LED_BRIGHTNESS = 5;

constexpr uint32_t EMPTY = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void initializeLED() {
  strip.begin();
  strip.show();
  strip.setBrightness(LED_BRIGHTNESS);
}

#ifdef TESTING
void setHeight(xy_t pos, long height) {}
#else
void setHeight(xy_t pos, long height) {
  // To deal with the silly LED snake numbering, we leave 1 empty row
  const int ledIndex = 16 * (pos.y  * 2) + pos.x;

  if (height == 0) {
    strip.setPixelColor(ledIndex, EMPTY);
  } else {
    strip.setPixelColor(ledIndex, strip.Color(150, 0, 0));
  }
  strip.show();
}
#endif

void displayLCD(const char *msg, int score) {}

#ifndef TESTING
xy_t randomMole() {
  return { random(0, 3), random(0, 3) };
}
#else
xy_t randomMole() {
  return { 1, 1 };
}
#endif

ButtonGrid readButtons() {
  return std::array<std::array<bool, 3>, 3>{};
}