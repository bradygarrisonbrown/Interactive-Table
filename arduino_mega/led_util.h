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


void initializeLED();

void setPWM(int min, int max, int val);

void setStripColor(int x, int y, NamedColors color);

void setGridColor(int sectionRow, int sectionCol, NamedColors color);

void fillStrip(NamedColors color);

void fillGrid(NamedColors color);

void displayLCD(const char *msg, int score);

void rainbowFade(int wait, int rainbowLoops);

#endif


