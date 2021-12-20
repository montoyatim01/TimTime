#ifndef __GLOBAL_H
#define __GLOBAL_H
#include <stdint.h>
#include "stm32l4xx_hal.h"
#define calibration 23999979


//Global variables//
extern volatile uint32_t clockFrame;
extern volatile bool tcJammed;

extern uint32_t displayTimeout;

extern uint8_t frameRate;

extern uint8_t hr, mn, sc, fr;

extern bool buttonsHeld;

extern ADC_HandleTypeDef *battADC;
extern I2C_HandleTypeDef *dispI2C;
extern I2C_HandleTypeDef *memI2C;
extern TIM_HandleTypeDef *inTIM;
extern TIM_HandleTypeDef *outTIM;
extern TIM_HandleTypeDef *countTIM;

/*Calibration*/
extern volatile uint16_t compensationCounter;
extern volatile bool compensateEnable;
//const uint32_t calibrationAInterval = 240000;
//extern int32_t calibrationA;
extern int32_t calibrationArray[6];
extern uint32_t calibrationInterval[6];
extern uint8_t frameRateDivisor[6];
extern uint16_t frameRateARR[6];

//extern volatile uint8_t tc[10];
extern volatile uint8_t tcIN[8];

extern uint8_t intOffset;

extern bool stat1;
extern bool stat2;

extern uint8_t autoOff;
extern uint16_t autoOffMinutes;
extern uint16_t uptimeMinutes;
extern float batteryRemaining;
extern uint8_t rateAdjust;
extern uint8_t offsetAdjust;


#endif

