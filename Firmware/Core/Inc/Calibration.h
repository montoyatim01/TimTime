#ifndef __CALIBRATION_H
#define __CALIBRATION_H
#include <stdint.h>

extern uint32_t newCal;
void calibrationMenu();
bool writeCalibration(uint32_t calibrationWrite);
void calibrationStatus();

#endif