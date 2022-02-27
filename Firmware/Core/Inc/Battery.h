#ifndef __BATTERY_H
#define __BATTERY_H
#include <stdint.h>


float batteryRead();
float battPercent(uint16_t analogReading);
#endif