#ifndef WDT_H
#define WDT_H

#include <avr/wdt.h>

static inline void initWDT() {
  wdt_enable(WDTO_4S);
}

static inline void petWDT() {
  wdt_reset();
}

#endif