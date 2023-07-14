#include "Battery.h"
#include "Global.h"
#include "stm32l4xx_hal.h"

float batteryRead()
{
    uint16_t averageReading = 0;
    //uint8_t bufferPosition = 0;
    uint16_t readBuffer[8];

    HAL_ADC_Start(battADC);
    HAL_ADC_PollForConversion(battADC, HAL_MAX_DELAY);
    for (int i=0; i<8; i++){
        readBuffer[i] = HAL_ADC_GetValue(battADC);
        averageReading += readBuffer[i];
    }
    batteryRemaining = (battPercent((averageReading / 8)));
    return (battPercent((averageReading / 8)));
    
    
}


float battPercent(uint16_t analogReading){
    if (analogReading >= 2700) {
        return 1.0;
    }
    if (analogReading >= 2675 && analogReading < 2700) {
        return 1.0;
    }
    if (analogReading >= 2650 && analogReading < 2675) {
        return ( (0.08*((float)analogReading)) - 115.0 )/100.0;
    }
    if (analogReading >= 2625 && analogReading < 2650) {
        return ( (0.12*((float)analogReading)) - 221.0 )/100.0;
    }
    if (analogReading >= 2600 && analogReading < 2625) {
        return ( (0.16*((float)analogReading)) - 326.0 )/100.0;
    }
    if (analogReading >= 2575 && analogReading < 2600) {
        return ( (0.24*((float)analogReading)) - 534.0 )/100.0;
    }
    if (analogReading >= 2540 && analogReading < 2575) {
        return ( (0.11428*((float)analogReading)) - 210.28 )/100.0;
    }
    if (analogReading >= 2525 && analogReading < 2540) {
        return ( (0.4*((float)analogReading)) - 936.0 )/100.0;
    }
    if (analogReading >= 2500 && analogReading < 2525) {
        return ( (0.36*((float)analogReading)) - 835.0 )/100.0;
    }
    if (analogReading >= 2470 && analogReading < 2500) {
        return ( (0.366*((float)analogReading)) - 851.66 )/100.0;
    }
    if (analogReading >= 2450 && analogReading < 2470) {
        return ( (1.1*((float)analogReading)) - 2663.0 )/100.0;
    }
    if (analogReading >= 2435 && analogReading < 2450) {
        return ( (0.9333*((float)analogReading)) - 2254.66 )/100.0;
    }
    if (analogReading >= 2400 && analogReading < 2435) {
        return ( (0.1714285*((float)analogReading)) - 399.0 )/100.0;
    }
    if (analogReading >= 2380 && analogReading < 2400) {
        return ( (0.2*((float)analogReading)) - 468.0 )/100.0;
    }
    if (analogReading >= 2344 && analogReading < 2380) {
        return ( (0.111111*((float)analogReading)) - 256.0 )/100.0;
    }
    if (analogReading >= 2320 && analogReading < 2344) {
        return ( (0.125*((float)analogReading)) - 289.0 )/100.0;
    }
    if (analogReading >= 2300 && analogReading < 2320) {
        return 0.0;
    }
    if (analogReading >= 2260 && analogReading < 2300) {
        return 0.0;
    }
    if (analogReading >= 1900 && analogReading < 2260) {
        return 0.0;
    }
    if (analogReading < 1900){
        return 0.0;
    }
    


}