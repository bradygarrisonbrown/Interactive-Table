#include "LED_util.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// NeoPixel setup
#define LED_PIN     13
#define GRID_COUNT   256
#define STRIP_COUNT   28
#define BRIGHTNESS  5


Adafruit_NeoPixel strip(STRIP_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel strip(GRID_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


//LED Strip DEfinitions

const uint8_t group_1_1[] = {1,7,8};
const uint8_t group_1_2[] = {2,6};
const uint8_t group_1_3[] = {3,4,5};

const uint8_t group_2_1[] = {9,17,18};
const uint8_t group_2_2[] = {10,11,16};
const uint8_t group_2_3[] = {12,13,14,15};

const uint8_t group_3_1[] = {19,20,27};
const uint8_t group_3_2[] = {21,25,26};
const uint8_t group_3_3[] = {22,23,24};

struct LEDGroup {
  const uint8_t* indices;
  uint8_t count;
};

LEDGroup ledMap[3][3] = {
  { {group_1_1, sizeof(group_1_1)}, {group_1_2, sizeof(group_1_2)}, {group_1_3, sizeof(group_1_3)} },
  { {group_2_1, sizeof(group_2_1)}, {group_2_2, sizeof(group_2_2)}, {group_2_3, sizeof(group_2_3)} },
  { {group_3_1, sizeof(group_3_1)}, {group_3_2, sizeof(group_3_2)}, {group_3_3, sizeof(group_3_3)} }
};


// --- LED Matrix and Section Definitions ---
#define MATRIX_SIZE 16         // 16x16 grid = 256 pixels
#define NUM_SECTIONS_PER_SIDE 3 // 3x3 grid = 9 sections total

// Base dimensions for the 3x3 grid sections (5x5, 5x6, 6x5, 6x6)
const int BASE_ROWS = MATRIX_SIZE / NUM_SECTIONS_PER_SIDE; // 16 / 3 = 5
const int LAST_ROWS = MATRIX_SIZE - (BASE_ROWS * 2);       // 16 - 10 = 6 


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
void setStripColor(int x, int y, NamedColors color) {
    if (x < 1 || x > 3 || y < 1 || y > 3) return;  // safety

    LEDGroup group = ledMap[x - 1][y - 1];

    uint32_t packedColor = getColorValue(color);
    for (uint8_t i = 0; i < group.count; i++) {
        uint8_t ledIndex = group.indices[i];
        strip.setPixelColor(ledIndex, packedColor);
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
    //for (int i = 0; i < LED_COUNT; i++) {
    for (int i = 1; i < STRIP_COUNT; i++) {
      strip.setPixelColor(i, packedColor);
    }
    strip.show();  // Only call once, after filling all pixels
}

void displayLCD(const char *msg, int score) {}