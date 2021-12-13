#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "main.h"
#include "Menu.h"
typedef enum{
    d_off = 0x0,
    d_home = 0x1,
    d_menu = 0x2,
    d_lock = 0x3,
    d_power = 0x4,
    d_rateWarn = 0x5,
    d_offsetWarn = 0x6,
    d_rejamWarn = 0x7
} displayMode;
//typedef enum displayMode displayMode;



#else
void updateDisplay(uint8_t state);
void displayOff();
void displayMain();
void displayLocked();
void displayPower();
void displayMenu();
void displayConfirmationRateChange();
void displayConfirmationOffsetChange();
void displayConfirmationRejam();
void displayBattery();
void timecodeDisplay();
void initDisplay();

extern char tcDisplay[11];
#endif
