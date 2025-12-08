#ifndef FSM_UTIL_H
#define FSM_UTIL_H

#include "shared.h"

#ifndef TESTING
xy_t randomMole() {
  return { random(0, 3), random(0, 3) };
  // return { 0, 0};
}
#else
xy_t randomMole() {
  return { 1, 1 };
}
#endif

#endif
