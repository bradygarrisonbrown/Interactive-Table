#ifndef LED_util_H
#define LED_util_H

/*
LED HANDLING CODE
*/

// Define the named colors as constants
enum NamedColors {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_WHITE,
    COLOR_BLACK // Turns the section off
};

namespace MoleColors {
    inline constexpr NamedColors RAISED = COLOR_MAGENTA;
    inline constexpr NamedColors HIT = COLOR_GREEN;
    inline constexpr NamedColors EXPIRED = COLOR_BLUE;
    inline constexpr NamedColors MISSED = COLOR_RED;
    inline constexpr NamedColors CLEAR = COLOR_BLACK;
}


/**
 * Initialize LED hardware.
 *
 * Inputs: None.
 * Outputs: None.
 * Side Effects: Clears LED strip + array.
 */
void initializeLED();

/**
 * Sets the round PWM led.
 *
 * Inputs: See function signature.
 * Outputs: None.
 */
void setPWM(int min, int max, int val);

/**
 * Sets the cell color on the strip. May set multiple LEDs because of our wraparound
 * hardware design.
 *
 * Inputs: See function signature.
 * Outputs: None.
 */
void setStripColor(int x, int y, NamedColors color);

/**
 * Sets the color of a grid cell.
 *
 * Inputs: See function signature.
 * Outputs: None.
 */
void setGridColor(int sectionRow, int sectionCol, NamedColors color);

/**
 * Sets all columns to a color via the strip.
 *
 * Inputs: See function signature.
 * Outputs: None.
 */
void fillStrip(NamedColors color);

/**
 * Sets the whole grid to a single color.
 *
 * Inputs: See function signature.
 * Outputs: None.
 */
void fillGrid(NamedColors color);

// DEPRECATED
void displayLCD(const char *msg, int score);

/**
 * Set up a rainbow animation on LED grid. 
 *
 * Inputs: Wait is the time to pause in addition to a default 500ms pause.
 * Outputs: None.
 */
void rainbowFade(int wait, int rainbowLoops);

#endif


