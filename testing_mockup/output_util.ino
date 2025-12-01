#include "output_util.h"

#include "Arduino_LED_Matrix.h"
#include <array>

constexpr int LED_PIN = 8;
constexpr int LED_COUNT = 256;
constexpr int LED_BRIGHTNESS = 5;

constexpr uint32_t EMPTY = 0;

ArduinoLEDMatrix ledMatrix;

void initializeLED() {
  ledMatrix.begin();
}

#ifdef TESTING
void setHeight(xy_t pos, long height) {}
#else
constexpr int LED_HEIGHT = 12;
constexpr int LED_WIDTH = 8;
std::array<std::array<byte, LED_HEIGHT>, LED_WIDTH> frame_{};
void setHeight(xy_t pos, long height) {
  if (height == 0) {
    frame_[pos.y][pos.x] = 0;
  } else {
    frame_[pos.y][pos.x] = 1;
  }
  ledMatrix.renderBitmap(frame_.data(), LED_HEIGHT, LED_WIDTH);
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