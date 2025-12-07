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

void initializeLED();

void setStripColor(int x, int y, NamedColors color);

void setGridColor(int sectionRow, int sectionCol, NamedColors color);

void fillLEDs(NamedColors color);

void displayLCD(const char *msg, int score);

#endif


