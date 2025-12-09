#ifndef FSM_UTIL_H
#define FSM_UTIL_H

#include "shared.h"

#ifndef TESTING
xy_t randomMole() {
  static constexpr int PRESET_MOLES_SIZE = 10;
  static constexpr xy_t moles[PRESET_MOLES_SIZE] = {{0, 0}, {0, 2}, {2, 2}, {1, 0}, {1, 2}, {2, 1}, {0, 0}, {0, 1}, {0, 1}, {1, 0}};
  static int currentIndex = 0;

  const auto chosen = moles[currentIndex];
  currentIndex = (currentIndex + 1) % PRESET_MOLES_SIZE;
  return chosen;
  // return { random(0, 3), random(0, 3) };
  // return { 0, 0};
}
#else
xy_t randomMole() {
  return { 1, 1 };
}
#endif

#endif
