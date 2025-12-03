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

// coordinates in row, col
constexpr int BUTTON_1_PIN = 0; // 0,0
constexpr int BUTTON_2_PIN = 1; // 0,1
constexpr int BUTTON_3_PIN = 2; // 0,2
constexpr int BUTTON_4_PIN = 3; // 1,0
constexpr int BUTTON_5_PIN = 4; // 1,1
constexpr int BUTTON_6_PIN = 5; // 1,2
constexpr int BUTTON_7_PIN = 6; // 2,0
constexpr int BUTTON_8_PIN = 7; // 2,1
constexpr int BUTTON_9_PIN = 8; // 2,2

ButtonGrid readButtons() {
  auto grid = std::array<std::array<bool, 3>, 3>{};
  for (int y = 0; y < Constants::HEIGHT; ++y) {
    for (int x = 0; x < Constants::WIDTH; ++x) {
      const auto buttonIndex = y * Constants::HEIGHT + x;
      if (digitalRead(buttonIndex) == HIGH) {
        grid[y][x] = true;
      }
    }
  }
  return grid;
}