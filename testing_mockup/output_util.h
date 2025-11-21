#pragma once

#include "shared.h"
#include <array>

void initializeLED();

void setHeight(xy_t pos, long height);

void displayLCD(const char *msg, int score);

xy_t randomMole();

ButtonGrid readButtons();