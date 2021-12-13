#ifndef __MENU_H
#define __MENU_H
#include <stdint.h>




void menuLoop();
void setAutoOff();
void rateAlert();
void offsetAlert();
void reJamAlert();

extern bool highlightYes;
extern bool menuItemSelect;
extern uint8_t menuItem;

#endif