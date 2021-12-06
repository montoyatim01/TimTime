#include "Battery.h"

uint16_t batteryInit(ADC_HandleTypeDef *hadc1){
    uint16_t averageReading = 0;
    bufferPosition = 0;
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    for int(i=0; i<readBuffer.count(); i++){
        readBuffer[i] = HAL_ADC_GetValue(&hadc1);
        averageReading += readBuffer[i];
    }
    
    
}