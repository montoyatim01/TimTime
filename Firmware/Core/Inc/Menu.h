#ifndef __MENU_H
#define __MENU_H
#include <stdint.h>




void menuLoop();
void setAutoOff();
void rateAlert();
void offsetAlert();
void reJamAlert();
void userBitMenu();

extern bool highlightYes;
extern bool menuItemSelect;
extern uint8_t menuItem;
extern uint8_t displayUserBits[8];


#endif