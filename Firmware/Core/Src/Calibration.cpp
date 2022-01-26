#include "Calibration.h"
#include "Display.h"
#include "stm32l4xx_hal.h"
#include "Global.h"
#include "Menu.h"
uint32_t newCal = calibration;


bool calibrateSuccess = false;
/*bool menuItemSelect = false;
uint8_t menuItem = 0;
bool highlightYes = false;*/


//00 Number
//01 OK
//02 Cancel
void calibrationMenu(){
    uint32_t buttonTime;
    bool inMenu = true;
    menuItemSelect = false;
    
    updateDisplay(d_calibration);
    while(GPIOB->IDR & GPIO_PIN_8 || GPIOB->IDR & GPIO_PIN_9){
        //Do nothing while holding buttons
    }
    displayTimeout = HAL_GetTick();
    while (inMenu){
        updateDisplay(d_calibration);
        if (HAL_GetTick() - displayTimeout > 30000){
            inMenu = false;
        }


        if (GPIOB->IDR & GPIO_PIN_9)
        { //Plus button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                if (menuItemSelect)
                { //Menu item selected, scrolling through values
                    switch (menuItem)
                    {
                    case 0: //Number
                        if (newCal == 23000000)
                            newCal = 23000000;
                        else
                            newCal--;
                        break;
                    case 1: //OK
                        break;
                    case 2: //Cancel
                        break;
                    case 3:
                        break;
                        
                        
                    }
                }
                else
                { //Scrolling through menu
                    menuItem++;
                    if (menuItem > 2)
                        menuItem = 0;
                }
                buttonTime = HAL_GetTick(); //Debounce timer
            }
        }

        if (GPIOB->IDR & GPIO_PIN_8)
        { //Minus button

            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                if (menuItemSelect)
                { //Scrolling through option
                    switch (menuItem)
                    {
                    case 0: //Number
                        newCal++;
                        if (newCal > 25000000)
                            newCal = 25000000;
                        break;
                        
                    case 1: //OK
                        break;
                    case 2: //Cancel
                        break;
                    }
                }
                else
                { //Scrolling through menu
                    if (menuItem == 0)
                        menuItem = 2;
                    else
                        menuItem--;
                }
                buttonTime = HAL_GetTick(); //Debounce timer
            }
        }

        if (GPIOC->IDR & GPIO_PIN_13)
        { //Menu button
            displayTimeout = HAL_GetTick();
            if (!(GPIOB->IDR & GPIO_PIN_8))
            {

                displayTimeout = HAL_GetTick();
                if (HAL_GetTick() - buttonTime > 350) //Debounce
                {
                    if (!menuItemSelect)
                    { //if scrolling through menu items
                        switch (menuItem)
                        {
                        case 0: //Number
                            menuItemSelect = true;
                            break;
                        case 1: //OK
                            calibrateSuccess = writeCalibration(newCal);
                            //calibration = newCal;
                            calibrationStatus();

                            inMenu = false;
                            //Try writing cal
                            //Exit menu
                            menuItemSelect = true;
                            //offsetAlert();
                            while (GPIOC->IDR & GPIO_PIN_13)
                            {displayTimeout = HAL_GetTick();}
                            break;
                        case 2: //Cancel
                            inMenu = false;
                            while (GPIOC->IDR & GPIO_PIN_13)
                            {displayTimeout = HAL_GetTick();}
                            break;
                        }
                    }
                    else //Exit menu option
                    {
                        switch (menuItem)
                        {
                        case 0: //Rate
                            menuItemSelect = false;
                            //calibration = newCal;
                            break;
                        }
                    }
                    buttonTime = HAL_GetTick(); //Debounce timer
                }
            }
        }


    }
}


/* Write Calibration
* Write the values of the user inputted calibration
* to the EEPROM
*
*/
bool writeCalibration(uint32_t calibrationWrite){
HAL_Delay(10);
uint8_t calWrite[4];
calWrite[0] = calibrationWrite;
calWrite[1] = calibrationWrite >> 8;
calWrite[2] = calibrationWrite >> 16;
calWrite[3] = calibrationWrite >> 24;
for (int i=0; i<4; i++){
	if(HAL_I2C_Mem_Write(memI2C , 0x50<<1, 0x0005+i, 1, &calWrite[i], 1,1000)!= HAL_OK)	//offset
				{
						  return false;
				}
				 HAL_Delay(10);
}
        
        return true;
}

void calibrationStatus(){
    menuItemSelect = false;
    bool inScreen = true;
    if (calibrateSuccess){
        calibration = newCal;
        updateDisplay(d_calOK);
    } else {
        updateDisplay(d_calFail);
    }
    while(GPIOC->IDR & GPIO_PIN_13){
        //do nothing
    }
    if (calibrateSuccess){
        while(inScreen){
            updateDisplay(d_calOK);
            if (GPIOC->IDR & GPIO_PIN_13) //Menu button
            {
                menuItemSelect = true;
                updateDisplay(d_calOK);
                inScreen = false; 
            }
        }
        HAL_Delay(500);
        
    }
    else {
        while(inScreen){
            updateDisplay(d_calFail);
        if (GPIOC->IDR & GPIO_PIN_13) //Menu button
            {
                menuItemSelect = true;
                updateDisplay(d_calFail);
                inScreen = false; 
            }
        }
        HAL_Delay(500);
    }
}