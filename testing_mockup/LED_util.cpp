#include "LED_util.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// NeoPixel setup
#define LED_PIN     6
#define LED_COUNT   256
#define BRIGHTNESS  5

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// --- LED Section Definitions ---
#define NUM_SECTIONS 9
#define LEDS_PER_BASE_SECTION (LED_COUNT / NUM_SECTIONS) // 256 / 9 = 28
#define LEDS_IN_LAST_SECTION (LED_COUNT - (LEDS_PER_BASE_SECTION * (NUM_SECTIONS - 1))) // 32 LEDs

// --- LED Matrix and Section Definitions ---
#define MATRIX_SIZE 16         // 16x16 grid = 256 pixels
#define NUM_SECTIONS_PER_SIDE 3 // 3x3 grid = 9 sections total

// Base dimensions for the 3x3 grid sections (5x5, 5x6, 6x5, 6x6)
const int BASE_ROWS = MATRIX_SIZE / NUM_SECTIONS_PER_SIDE; // 16 / 3 = 5
const int LAST_ROWS = MATRIX_SIZE - (BASE_ROWS * 2);       // 16 - 10 = 6 

// Array defining the STARTING index of each section
const int sectionStartIndices[NUM_SECTIONS] = {
    0,                                      // Section 1 (Start 0)
    LEDS_PER_BASE_SECTION * 1,              // Section 2 (Start 28)
    LEDS_PER_BASE_SECTION * 2,              // Section 3 (Start 56)
    LEDS_PER_BASE_SECTION * 3,              // Section 4 (Start 84)
    LEDS_PER_BASE_SECTION * 4,              // Section 5 (Start 112)
    LEDS_PER_BASE_SECTION * 5,              // Section 6 (Start 140)
    LEDS_PER_BASE_SECTION * 6,              // Section 7 (Start 168)
    LEDS_PER_BASE_SECTION * 7,              // Section 8 (Start 196)
    LEDS_PER_BASE_SECTION * 8               // Section 9 (Start 224)
};


// Helper function to convert the NamedColor enum to a 32-bit color value
uint32_t getColorValue(NamedColors color) {
    switch (color) {
        case COLOR_RED:
            return strip.Color(255, 0, 0);
        case NamedColors::COLOR_GREEN:
            return strip.Color(0, 255, 0);
        case COLOR_BLUE:
            return strip.Color(0, 0, 255);
        case COLOR_YELLOW:
            return strip.Color(255, 255, 0);
        case COLOR_CYAN:
            return strip.Color(0, 255, 255);
        case COLOR_MAGENTA:
            return strip.Color(255, 0, 255);
        case COLOR_WHITE:
            return strip.Color(255, 255, 255);
        case COLOR_BLACK:
            return strip.Color(0, 0, 0);
        default:
            return strip.Color(0, 0, 0); // Default to black
    }
}

void initializeLED() {

    // Neopixel init
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
    #endif

    strip.begin();
    strip.show(); // Turn off all pixels initially
    strip.setBrightness(BRIGHTNESS);
}

// Sets a specific section of the LED strip to a single color using the enum
// sectionNumber: 1 to 9
// color: A value from the NamedColors enum
void setSectionColor(int sectionNumber, NamedColors color) {
    if (sectionNumber < 1 || sectionNumber > NUM_SECTIONS) return;

    // Convert section number (1-9) to array index (0-8)
    int index = sectionNumber - 1; 

    int startIndex = sectionStartIndices[index];
    int sectionLength;

    // Determine the length for the current section
    if (index == NUM_SECTIONS - 1) { 
        sectionLength = LEDS_IN_LAST_SECTION;
    } else {
        sectionLength = LEDS_PER_BASE_SECTION;
    }
    
    int endIndex = startIndex + sectionLength;

    // Get the 32-bit color value using the new helper function
    uint32_t packedColor = getColorValue(color);

    // Loop only through the pixels in the target section
    for (int i = startIndex; i < endIndex; i++) {
        strip.setPixelColor(i, packedColor);
    }

    // Note: strip.show() is called to update the strip

    strip.show();
}

// Function to convert 2D (x,y) coordinates to 1D NeoPixel index
// ASSUMES SERPENTINE WIRING: Even rows L->R, Odd rows R->L.
int xyToStripIndex(int x, int y) {
  if (y % 2 == 0) { 
    return y * MATRIX_SIZE + x;
  } else { 
    return (y + 1) * MATRIX_SIZE - 1 - x;
  }
}

// Sets a specific grid section (1-9) to a single color using the enum
void setGridColor(int sectionRow, int sectionCol, NamedColors color) {
    // There are 9 sections (1-9)
    //if (sectionNumber < 1 || sectionNumber > 9) return;

    // 1. Convert Section Number (1-9) to Grid Coordinates (row 0-2, col 0-2)
    //int sectionRow = (sectionNumber - 1) / NUM_SECTIONS_PER_SIDE; // 0, 1, or 2
    //int sectionCol = (sectionNumber - 1) % NUM_SECTIONS_PER_SIDE; // 0, 1, or 2




    // 2. Determine Start/End Coordinates for this Section

    // Y-Coordinates (Rows)
    int startY, endY;
    if (sectionRow < 2) {
        startY = sectionRow * BASE_ROWS;
        endY = startY + BASE_ROWS;
    } else { // Last row (Section 7, 8, 9) gets the extra 6 pixels
        startY = 2 * BASE_ROWS;
        endY = startY + LAST_ROWS; // endY will be 16
    }

    // X-Coordinates (Columns)
    int startX, endX;
    if (sectionCol < 2) {
        startX = sectionCol * BASE_ROWS;
        endX = startX + BASE_ROWS;
    } else { // Last column (Section 3, 6, 9) gets the extra 6 pixels
        startX = 2 * BASE_ROWS;
        endX = startX + LAST_ROWS; // endX will be 16
    }
    
    // 3. Apply Color to Pixels
    uint32_t packedColor = getColorValue(color);

    // Nested loop iterates through the calculated square boundaries
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            int i = xyToStripIndex(x, y); // Convert 2D coord to 1D strip index
            strip.setPixelColor(i, packedColor);
        }
    }

    strip.show();
}


// Sets all of the LED strip to a single color using the enum
// sectionNumber: 1 to 9
// color: A value from the NamedColors enum
void fillLEDs(NamedColors color){
      // Fill all LEDs with chosen color

    uint32_t packedColor = getColorValue(color);
    strip.clear();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, packedColor);
    }
    strip.show();  // Only call once, after filling all pixels
}

void displayLCD(const char *msg, int score) {}