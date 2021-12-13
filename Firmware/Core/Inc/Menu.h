#ifndef __MENU_H
#define __MENU_H

#include "main.h"
#include "stm32l4xx_hal.h"
#include "Display.h"
#include "Timecode.h"

#else
void menuLoop();
void setAutoOff();
void rateAlert();
void offsetAlert();
void reJamAlert();

extern bool highlightYes;
extern bool menuItemSelect;
extern uint8_t menuItem;

#endif