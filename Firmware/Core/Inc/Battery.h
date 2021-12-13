#ifndef __BATTERY_H
#define __BATTERY_H
#include "stm32l4xx_hal.h"
#include "main.h"

#else
float batteryRead();
float battPercent(uint16_t analogReading);
#endif