#ifndef WDT_H
#define WDT_H

#include <avr/wdt.h>

/**
 * Initialize WDT
 *
 * Inputs: None.
 * Outputs: None.
 * Side Effects: Initialize and start watchdog.
 */
static inline void initWDT() {
  wdt_enable(WDTO_4S);
}

/**
 * Pet the WDT
 *
 * Inputs: None.
 * Outputs: None.
 * Side Effects: Resets the WDT timer.
 */
static inline void petWDT() {
  wdt_reset();
}

#endif