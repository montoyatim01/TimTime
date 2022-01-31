#include "Menu.h"
#include "Display.h"
#include "Timecode.h"
#include "stm32l4xx_hal.h"
#include "Global.h"

uint32_t buttonTime;
uint8_t rateAdjust;
uint8_t offsetAdjust;
bool menuItemSelect = false;
uint8_t menuItem = 0;
bool highlightYes = false;
uint8_t displayUserBits[8] = {0};
void menuLoop()
{
    bool inMenu = true;
    rateAdjust = frameRate;
    offsetAdjust = intOffset;
    
    /*
	 * Check button press. If menu item selected, advance the option
	 * if menu item not selected, advace through menu
	 * update display throughout
	 * check if item is exit
	 * timeout for menu inactivity 10 seconds?
	 */
    

    displayTimeout = HAL_GetTick();
    while (GPIOC->IDR & GPIO_PIN_13)
    {
        //While still holding down the menu button
    }
    while (inMenu)
    {
        updateDisplay(2);
        if (HAL_GetTick() - displayTimeout > 10000)
        {
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
                    case 0: //Rate
                        if (rateAdjust == 0)
                            rateAdjust = 5;
                        else
                            rateAdjust--;
                        break;
                    case 1: //Offset
                        if (offsetAdjust == 0)
                            offsetAdjust = 0;
                        else
                            offsetAdjust--;
                        break;
                    case 2:
                        break;
                    case 3:
                        if (autoOff == 0)
                            autoOff = 4;
                        else
                            autoOff--;
                        break;
                        
                        
                    }
                }
                else
                { //Scrolling through menu
                    menuItem++;
                    if (menuItem > 4)
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
                    case 0:
                        rateAdjust++;
                        if (rateAdjust > 5)
                            rateAdjust = 0;
                        break;
                        
                    case 1:
                        offsetAdjust++;
                        if (offsetAdjust > 60)
                            offsetAdjust = 60;
                        break;
                    case 2:
                        break;
                    case 3:
                        autoOff++;
                        if (autoOff > 4)
                            autoOff = 0;
                        break;
                    }
                }
                else
                { //Scrolling through menu
                    if (menuItem == 0)
                        menuItem = 4;
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
                        case 0: //Rate
                            menuItemSelect = true;
                            break;
                        case 1: //Offset
                            menuItemSelect = true;
                            //offsetAlert();
                            ///while (GPIOC->IDR & GPIO_PIN_13)
                            //{displayTimeout = HAL_GetTick();}
                            break;
                        case 2: //Re-jam
                            reJamAlert();
                            while (GPIOC->IDR & GPIO_PIN_13)
                            {displayTimeout = HAL_GetTick();}
                            break;
                        case 3: //Auto-Off
                            menuItemSelect = true;
                            break;
                        case 4: //Exit menu
                            buttonTime = HAL_GetTick();
                            //menuCount = HAL_GetTick();
                            //GPIOH->ODR |= GPIO_PIN_3; //Not sure what this does
                            //Write to EEPROM
                            inMenu = false;
                            uptimeMinutes = 0;
                            break;
                        }
                    }
                    else //Exit menu option
                    {
                        switch (menuItem)
                        {
                        case 0: //Rate
                            menuItemSelect = false;
                            rateAlert();
                            while (GPIOC->IDR & GPIO_PIN_13)
                            {displayTimeout = HAL_GetTick();}
                            rateAdjust = frameRate;
                            break;
                        case 1: //Offset
                            offsetAlert();
                            while (GPIOC->IDR & GPIO_PIN_13)
                            {displayTimeout = HAL_GetTick();}
                            menuItemSelect = false;
                            break;
                        case 2: //Re-Jam
                            reJamAlert();
                            menuItemSelect = false;
                            break;
                        case 3: //Auto-Off
                            menuItemSelect = false;
                            break;
                        }
                    }
                    buttonTime = HAL_GetTick(); //Debounce timer
                }
            }
        }
    }
    updateDisplay(0x0);
}

void setAutoOff()
{
    switch (autoOff)
    {
    case 0:
        autoOffMinutes = 60;
        break;
    case 1:
        autoOffMinutes = 120;
        break;
    case 2:
        autoOffMinutes = 240;
        break;
    case 3:
        autoOffMinutes = 480;
        break;
    case 4:
        autoOffMinutes = 720;
        break;
    }
}

void rateAlert()
{
    bool inLoop = true;
    highlightYes = false;
    updateDisplay(0x5);
    while (GPIOC->IDR & GPIO_PIN_13)
    {
        //While still holding down the menu button
    }
    while (inLoop)
    {
        updateDisplay(0x5);
        if (HAL_GetTick() - displayTimeout > 5000)
        {
            highlightYes = false;
            inLoop = false;
        }

        if (GPIOB->IDR & GPIO_PIN_9)
        { //Plus button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                highlightYes = !highlightYes;
                buttonTime = HAL_GetTick(); //Debounce timer
            }
        }

        if (GPIOB->IDR & GPIO_PIN_8)
        { //Minus button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                highlightYes = !highlightYes;
                buttonTime = HAL_GetTick(); //Debounce timer
            }
        }

        if (GPIOC->IDR & GPIO_PIN_13)
        { //Menu button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                if (highlightYes)
                {
                    frameRate = rateAdjust;
                    resetTimecode();
                    highlightYes = false;
                    inLoop = false;
                    break;
                }
                inLoop = false;
                break;
                buttonTime = HAL_GetTick(); //Debounce timer
            }
        }
    }
}

void offsetAlert()
{
    bool inLoop = true;
    updateDisplay(0x6);
    while (GPIOC->IDR & GPIO_PIN_13)
    {
        //While still holding down the menu button
    }
    while (inLoop)
    {
        updateDisplay(0x6);
        if (HAL_GetTick() - displayTimeout > 8000)
        {
            highlightYes = false;
            inLoop = false;
        }

        if (GPIOC->IDR & GPIO_PIN_13)
        { //Menu button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                //Add/subtract offset to current frame?
                //Set offset for future jams?
                intOffset = offsetAdjust;
                highlightYes = !highlightYes;
                buttonTime = HAL_GetTick(); //Debounce timer
                inLoop = false;
            }
        }
    }
}

void reJamAlert()
{
    bool inLoop = true;
    highlightYes = false;
    updateDisplay(0x7);
    while (GPIOC->IDR & GPIO_PIN_13)
    {
        //While still holding down the menu button
    }
    while (inLoop)
    {
        updateDisplay(0x7);
        if (HAL_GetTick() - displayTimeout > 5000)
        {
            highlightYes = false;
            inLoop = false;
        }

        if (GPIOB->IDR & GPIO_PIN_9)
        { //Plus button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                highlightYes = !highlightYes;
                buttonTime = HAL_GetTick(); //Debounce timer
            }
        }

        if (GPIOB->IDR & GPIO_PIN_8)
        { //Minus button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                highlightYes = !highlightYes;
                buttonTime = HAL_GetTick(); //Debounce timer
            }
        }

        if (GPIOC->IDR & GPIO_PIN_13)
        { //Menu button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350) //Debounce
            {
                if (highlightYes)
                {
                    frameRate = rateAdjust;
                    resetTimecode();
                    highlightYes = !highlightYes;
                }
                buttonTime = HAL_GetTick(); //Debounce timer
                inLoop = false;
            }
        }
    }
}

void userBitMenu(){
    bool inMenu = true;
    menuItemSelect = false;
    highlightYes = false;
    for (int i=0; i<8; i++){
        displayUserBits[i] = userBits[i];
    }
    displayTimeout = HAL_GetTick();
    while (GPIOC->IDR & GPIO_PIN_13){
        //while still holding down the button
    }
    while (inMenu){
        updateDisplay(d_userBits);
        if (HAL_GetTick() - displayTimeout > 8000){
            inMenu = false;
            updateDisplay(d_off);
        }
        if (GPIOB->IDR & GPIO_PIN_9){   //Plus button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350){
                if (menuItemSelect && highlightYes){ 
                    //Changing digits
                    if (displayUserBits[menuItem] == 0){
                        displayUserBits[menuItem] = 9;
                    } else {
                        displayUserBits[menuItem]--;
                    }
                }
                if (menuItemSelect && !highlightYes){
                    //Changing digit position
                    if (menuItem == 0){
                        menuItem = 8;
                    } else {
                        menuItem--;
                    }
                    
                }
                if (!menuItemSelect){
                    //Cycle through menu
                    menuItem++;
                    if (menuItem > 2){
                        menuItem = 0;
                    }
                }
                buttonTime = HAL_GetTick();
            }

        }
        if (GPIOB->IDR & GPIO_PIN_8){   //Minus button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350){
                if (menuItemSelect && highlightYes){ 
                    //Changing digits
                    displayUserBits[menuItem]++;
                    if (displayUserBits[menuItem] > 9){
                        displayUserBits[menuItem] = 0;
                    }
                    
                }
                if (menuItemSelect && !highlightYes){
                    //Changing digit position
                    menuItem++;
                    if (menuItem > 8){
                        menuItem = 0;
                    }
                    
                }
                if (!menuItemSelect){
                    //Cycle through menu
                    if (menuItem == 0){
                        menuItem = 2;
                    } else {
                        menuItem--;
                    }
                }
                buttonTime = HAL_GetTick();
            }

        }
        if (GPIOC->IDR & GPIO_PIN_13){   //Menu button
            displayTimeout = HAL_GetTick();
            if (HAL_GetTick() - buttonTime > 350){
                if (menuItemSelect && highlightYes){ 
                    //Changing digits
                    highlightYes = false;
                }
                else if (menuItemSelect && !highlightYes){
                    //Changing digit position
                    if (menuItem == 8){
                        menuItemSelect = false;
                        highlightYes = false;
                        menuItem = 0;
                    }
                    else {
                        highlightYes = true;
                        
                    }
                    
                }
                else if (!menuItemSelect && !highlightYes){
                    //Cycle through menu
                    
                    if (menuItem == 0){
                        menuItemSelect = true;
                    }
                    if (menuItem == 1){
                        for (int i=0; i<8; i++){
                            userBits[i] = displayUserBits[i];
                        }
                        inMenu = false;
                        updateDisplay(0);
                        updateDisplay(d_home);
                        while (GPIOC->IDR & GPIO_PIN_13)
                            {
                                //While still holding down the menu button
                            }

                    }
                    if (menuItem == 2){
                        //Cancel
                        inMenu = false;
                        updateDisplay(0);
                        updateDisplay(d_home);
                        while (GPIOC->IDR & GPIO_PIN_13)
                            {
                                //While still holding down the menu button
                            }
                    }
                }
                buttonTime = HAL_GetTick();
            }

        }
    }
}
