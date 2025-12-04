#ifndef BUTTON_UTIL_H
#define BUTTON_UTIL_H

/*
BUTTON HANDLING CODE
*/

//Initialize button + emergency stop pin settings
void initializeButtons();

//Runs check button on each button and returns 3x3 array of boolean button status
ButtonGrid readButtons();

#endif
