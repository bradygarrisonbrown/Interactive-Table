#include "output_util.h"

void setHeight(xy_t pos, long height) {}

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