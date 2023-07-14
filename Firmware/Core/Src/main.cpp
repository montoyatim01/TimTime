/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "Global.h"
#include "Timecode.h"
#include "Menu.h"
#include "Display.h"
#include "Battery.h"
#include "Calibration.h"
#define EEPROM_ADDRESS	0xA0
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DAC_HandleTypeDef hdac1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim16;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */
//void setDigit(int addr, int digit, uint8_t value, bool dp);
//void spiTransfer(int addr, volatile uint8_t opcode, volatile uint8_t data);
//void transposeData(int addr);
void shiftRight(uint8_t theArray[], uint8_t theArraySize);
void initTimecode();
void clearBuffer(uint8_t theArray[], uint8_t theArraySize);
void writeTimecode();
bool readEEPROM();
bool updateEEPROM();

//void updateDisplay(bool on);
//void calibrate();
//void JumpToBootloader(void);
//void updateDisplay(uint8_t state);

//void write_eeprom_reg(uint16_t reg_addr, uint8_t value);
//uint8_t  read_eeprom_reg(uint16_t addr);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DAC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM16_Init(void);
static void MX_USB_PCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

ADC_HandleTypeDef *battADC = &hadc1;
I2C_HandleTypeDef *dispI2C = &hi2c2;
I2C_HandleTypeDef *memI2C=  &hi2c1;
TIM_HandleTypeDef *inTIM = &htim2;
TIM_HandleTypeDef *outTIM = &htim16;
TIM_HandleTypeDef *countTIM = &htim7;


//int Offset, hr, mn, sc, fr,setA,setB;
//volatile uint32_t clockFrame = 0;
uint32_t clockFrameOutput = 0;
uint8_t tc[10] = {0}; // ISR Buffer to store incoming bits
//uint32_t clockFramePrevious = 0;
volatile uint8_t jamCount = 0;

bool anodeMode = true;

const uint16_t sync = 0xBFFC; // Sync word to expect when running tape forward

enum flagBits {
  tcValid,        // TC copied to xtc is valid (Used by main loop to determing if a timecode has arrived)
  tcFrameError,   // ISR edge out of timing bounds (only gets reset after next valid TC read)
  tcOverrun,      // TC was not read from xtc by main loop before next value was ready (so main loop can tell if timecodes have been lost)
  tcForceUpdate,  // Valid TC will always be copied to buffer even if last value not read (if set by main code then TC will always be copied)
  tcHalfOne       // ISR is reading a 1 bit so ignore next edge (Internal to ISR)
};

volatile uint8_t tcFlags = 0;                             // Various flags used by ISR and main code
uint32_t uSeconds;                                        // ISR store of last edge change time

char timeCode[12];                                        // For example code another buffer to write decoded timecode
//char userBits[12];
volatile bool tcTimer = true;
volatile uint32_t edgeTimeDiff;
uint8_t tcWrite[10];
uint8_t tcWriteBuf[10];

//char tcDisplay[11];
char dispOffset[3];
//uint32_t buttonTime;


int prevOffset;



bool fullBit = false;
bool halfBit = false;
uint8_t writeState = 0x0;
int byteNumber = 0;
int bitNumber = 0;
uint8_t tcBuf;
bool endBit = false;


uint16_t batt;
uint16_t battAvg[32];
uint8_t battLoc = 0;
uint32_t battCount;
uint32_t powerCount;
uint32_t menuCount;
float battStatus;
char battDisp[5];
int battPrint;

//bool stat1;
//bool stat2;

uint8_t ledCount = 0;
volatile bool blink = false;

volatile uint16_t compensationCounter = 0;
volatile bool compensate = false;

uint8_t displayLoopCounter = 0;
uint32_t displayTimer;
uint32_t lockTimer;
bool isLocked = false;
bool displayOn = true;

bool powerOff = false;

uint8_t powerUpMode = 0;

bool calWrite = true;
bool calRead = true;
bool updateWrite = true;

//Lockit Prototype #2
//const uint32_t calibrationAInterval = 240000;
//int32_t calibrationA;
//const uint32_t calibration = 23999963;
//const uint32_t calibration = 23999979; //timtime1

//int32_t calibrationArray[6];
//calibrationArray[frameRate]

//const uint32_t calibrationInterval[6] = {240000,240000,250000,300000,300000,300000};
//calibrationInterval[frameRate]

/*
 * Lockit Prototype #1
 * const uint16_t calibrationAInterval = 24000;
 * const uint8_t calibrationBInterval = 50;
 * const uint8_t calibrationA = -860;
 * const uint8_t calibrationB = -43;
 */


/*
 * 0 = 23.976
 * 1 = 24
 * 2 = 25
 * 3 = 29.97
 * 4 = 30
 */
//uint8_t frRDv[6] = {24,24,25,30,30,30};	//Frame rate divisor array
//frRDv[frameRate]
//const uint16_t frARR[6] = {50049,49999,47999,40039,40039,39999};	//Frame rate Timer ARR
//frARR[frameRate]




//uint8_t lockCountdown = 0;
char powerDisplay;
char lockDisplay;
void mcStop();

bool upButton;
uint32_t upButtonTime;

bool downButton;
uint32_t downButtonTime;

bool menuButton;
uint32_t menuButtonTime;

uint32_t batteryCheck;
bool wakeUP;


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_DAC1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM16_Init();
  MX_USB_PCD_Init();
  /* USER CODE BEGIN 2 */
  DAC1->DHR12R2 = 2048; //Set analog out for TXCO VCO

  /*Configure GPIO pin : PC13 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



/*if(GPIOA -> IDR & GPIO_PIN_9 || GPIOA -> IDR & GPIO_PIN_10){  //Stat 1 & 2
  powerUpMode = 2;
}*/
wakeUP = false;
wakeUP = __HAL_PWR_GET_FLAG(PWR_FLAG_SB);
//clockFrame++;

if (!wakeUP){
  powerUpMode = 1;
  GPIOB -> ODR |= GPIO_PIN_12;    //LED
  HAL_Delay(500);
}
if(GPIOA -> IDR & GPIO_PIN_0){  //Power button
  uint32_t powerupTime = HAL_GetTick();
  while (GPIOA -> IDR & GPIO_PIN_0){
    if (HAL_GetTick() - powerupTime > 2000){
      powerUpMode = 1;
      GPIOB -> ODR |= GPIO_PIN_12;    //LED
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    }
  }
}

else {}
//do nothing

if (powerUpMode == 0){    //Power button not held long enough
HAL_Delay(500);

    HAL_PWR_EnterSTANDBYMode();
}

else if (powerUpMode == 2){
  //Run battery charge
  //In battery charge, continuously check power button
}

else {

}



HAL_Delay(20);
calRead = readEEPROM();
HAL_Delay(10);

//
    //LED
  //HAL_Delay(1000);
 // GPIOB -> ODR &= ~GPIO_PIN_12;  //LED
  //HAL_Delay(1000);
  //GPIOB -> ODR |= GPIO_PIN_12;  //LED
 // HAL_Delay(1000);

  //GPIOA -> ODR |= GPIO_PIN_8; //Power enable
     //Input timer
  //HAL_TIM_Base_Start(&htim7);
  //HAL_TIM_Base_Start_IT(&htim16);
  //HAL_TIM_Base_Start_IT(&htim6);
  tcWrite[8] = 0b11111100;  //Sync pattern
  tcWrite[9] = 0b10111111;  //Sync pattern




initDisplay();


if (GPIOB -> IDR & GPIO_PIN_8 && GPIOB -> IDR & GPIO_PIN_9){
  //Run Calibration menu
  calibrationMenu();
  calMenu = true;
}
if (!calRead && !calMenu){
  calReadFail();
}
calibrate();
HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3); //Input timer
updateDisplay(0x0);
updateDisplay(0x1);
 displayTimer = HAL_GetTick();
 batteryCheck = HAL_GetTick();
 batteryRead();
 batteryCheck = batteryCheck - 29900;
batteryRead();
  //HAL_Delay(3000);

     /* Clear the WU FLAG */
  //__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);


  //HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  //HAL_PWR_EnterSTANDBYMode();


//HAL_Delay(1);
//clockFrame++;
//HAL_TIM_Base_Start_IT(&htim7);		//Start the output timer
//HAL_TIM_Base_Start_IT(&htim16);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //JumpToBootloader();

    /* TODO
    * Only update display every 4th cycle?
    * Run tests on optimal update range
    * Timecode output is top priority
    *
    */
if (HAL_GetTick() - batteryCheck > 30000){
  batteryCheck = HAL_GetTick();
  batteryRead();
}
   if (!isLocked && displayOn){
     if (HAL_GetTick() - displayTimer > 10000){
       isLocked = true;
       displayOn = false;
       updateDisplay(0x0);
     }
     if (displayLoopCounter == 120){
      updateDisplay(0x1);
      displayLoopCounter = 0;
    }
   }

   if (isLocked && displayOn){
     if (HAL_GetTick() - displayTimer > 5000){
       displayOn = false;
       updateDisplay(0x0);
     }
     if (displayLoopCounter == 120){
      //updateDisplay(0x3);
      updateDisplay(d_lock);
      displayLoopCounter = 0;
    }
   }
   //Add in code to turn on display with button press?

   
  displayLoopCounter++;
frameCheck();
	  stat1 = GPIOA -> IDR & GPIO_PIN_9;
    stat2 = GPIOA -> IDR & GPIO_PIN_10;
	    //PA4 STat2
	    //PA5 Stat1
    //if (clockFrame == 2073600) clockFrame = 0;
    clockFrameOutput = clockFrame + 1;
	    tcWrite[0] = ((clockFrameOutput % frameRateDivisor[frameRate]) % 10);
	    tcWrite[0] |= (userBits[7] << 4);

	    tcWrite[1] = (clockFrameOutput % frameRateDivisor[frameRate]) / 10;
	    tcWrite[1] |= (userBits[6] << 4);

	    tcWrite[2] = ((clockFrameOutput / frameRateDivisor[frameRate]) % 60) % 10;
	    tcWrite[2] |= (userBits[5] << 4);

	    tcWrite[3] = ((clockFrameOutput / frameRateDivisor[frameRate]) % 60) / 10;
	    tcWrite[3] |= (userBits[4] << 4);

	    tcWrite[4] = ((clockFrameOutput / (frameRateDivisor[frameRate] * 60)) % 60) % 10;
	    tcWrite[4] |= (userBits[3] << 4);

	    tcWrite[5] = ((clockFrameOutput / (frameRateDivisor[frameRate] * 60)) % 60) / 10;
	    tcWrite[5] |= (userBits[2] << 4);

	    tcWrite[6] = (clockFrameOutput / (frameRateDivisor[frameRate] * 60 * 60)) % 10;
	    tcWrite[6] |= (userBits[1] << 4);

	    tcWrite[7] = (clockFrameOutput / (frameRateDivisor[frameRate] * 60 * 60)) / 10;
	    tcWrite[7] |= (userBits[0] << 4);

	    hr = (clockFrameOutput / frameRateDivisor[frameRate]) / 3600;
		mn = ((clockFrameOutput / frameRateDivisor[frameRate]) / 60 ) % 60;
		sc = (clockFrameOutput / frameRateDivisor[frameRate]) % 60;
		fr = clockFrameOutput % frameRateDivisor[frameRate];


	    //Determining the correct one bits to ensure
      //The wave is proper
      int oneBits = 0;
	    for (int i=0; i<10; i++){
	    	for (int b=0; b<8; b++){
	    		if (bitRead(tcWrite[i],b) == 1){
	    			oneBits++;
	    		}
	    	}
	    }
	    if (oneBits % 2 > 0){
	    	((tcWrite[7]) |= (1UL << (3)));
	    } else {
	    	((tcWrite[7]) &= ~(1UL << (3)));
	    }


	  

//button handlers
if(GPIOC -> IDR & GPIO_PIN_13){	//Menu button
//TODO Block entering menu when locked
/*displayTimer = HAL_GetTick();
	    	uint32_t menuCount = HAL_GetTick();
	    	//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	    	if (!isLocked){
	    	while (GPIOC -> IDR & GPIO_PIN_13){
	    		if (HAL_GetTick() - menuCount > 500){
	    			menuItem = 0;
	    			//inMenu = true;
	    			menuItemSelect = false;
					//menuEnter = true;
					updateDisplay(d_menu);
	    			menuLoop();
            displayTimer = HAL_GetTick();
            while (GPIOC -> IDR & GPIO_PIN_13)
              {updateDisplay(0x1);}//add in TC process?}
					//menuEnter = false;
	    		}
	    			updateDisplay(0x01);
	    	}
    }
    else {
      displayTimer = HAL_GetTick();
displayOn = true;
    }*/
    displayTimer = HAL_GetTick();
displayOn = true;
menuButton = true;
if(HAL_GetTick() - menuButtonTime > 1000 && !isLocked){
  menuItem = 0;
  menuItemSelect = false;
  updateDisplay(d_menu);
  menuLoop();
  //updateDisplay(d_userBits);
  //userBitMenu();
  updateWrite = updateEEPROM();
  displayTimer = HAL_GetTick();
  updateDisplay(0x1);
  menuButtonTime = HAL_GetTick();

}
	    }
      else {
        menuButton = false;
        menuButtonTime = HAL_GetTick();
      }

if(GPIOB -> IDR & GPIO_PIN_8){	//Up button
displayTimer = HAL_GetTick();
displayOn = true;
upButton = true;
if (HAL_GetTick() - upButtonTime > 2000){
  //User Bits
}

	    }
      else {
        upButton = false;
        upButtonTime = HAL_GetTick();
      }
if(GPIOB -> IDR & GPIO_PIN_9){	//Down button
displayTimer = HAL_GetTick();
displayOn = true;

	    }
      else{
        downButton = false;
        downButtonTime = HAL_GetTick();
      }

if(GPIOA -> IDR & GPIO_PIN_0){  //Power button
  if (!isLocked){
  updateDisplay(0x4);
  uint32_t currentTime = HAL_GetTick();
  while (GPIOA -> IDR & GPIO_PIN_0){
    powerCountdown =3 - ((HAL_GetTick() - currentTime)/1000);
    if (!powerOff){updateDisplay(0x4);}
    if (HAL_GetTick() - currentTime > 4000){
      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
      powerOff = true;
      updateDisplay(0x0);
       /* Clear the WU FLAG */
  //__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    }
  }
}
  
}
if(powerOff){
    //updateDisplay(0x0);
    HAL_Delay(2000);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnterSTANDBYMode();
    powerUpMode = 0;
    
}

if(isLocked && GPIOB -> IDR & GPIO_PIN_9 && GPIOB -> IDR & GPIO_PIN_8){//Both
  if (!upButton && !downButton){
  
  }
  upButton = true;
  downButton = true;
  buttonsHeld = true;
  /*
	    	while (isLocked && GPIOB -> IDR & GPIO_PIN_9 && GPIOB -> IDR & GPIO_PIN_8){
	    		updateDisplay(0x03);
          buttonsHeld = true;
	    		lockCountdown = 3 - ((HAL_GetTick() - lockTimer)/1000);
	    		if (HAL_GetTick()-lockTimer > 3000){
	    			isLocked = false;
	    			displayTimer = HAL_GetTick();
            buttonsHeld = false;
            
					uptimeMinutes = 0;
          updateDisplay(0x0);
	    		}
	    	}
         buttonsHeld = false;
	    	while (!isLocked && (GPIOB -> IDR & GPIO_PIN_9 || GPIOB -> IDR & GPIO_PIN_8)){
	    		updateDisplay(0x01);
	    		displayTimer = HAL_GetTick();
	    	}*/
        lockCountdown = 3 - ((HAL_GetTick() - lockTimer)/1000);
        updateDisplay(0x03);
        if(HAL_GetTick() - upButtonTime > 3000 && HAL_GetTick() - downButtonTime > 3000){
          isLocked = false;
	    			displayTimer = HAL_GetTick();
            buttonsHeld = false;
            uptimeMinutes = 0;
          updateDisplay(0x0);
        }

        

}
else {
  buttonsHeld = false;
  lockTimer = HAL_GetTick();
}



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 2;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 10;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */
  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00506682;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x0010030D;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /** I2C Fast mode Plus enable
  */
  HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C2);
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 24-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 24-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 19;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 50049;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 4-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 3128-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TC_OUT_Pin|LED_STATUS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TC_OUT_Pin LED_STATUS_Pin */
  GPIO_InitStruct.Pin = TC_OUT_Pin|LED_STATUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : OSC_Pin */
  GPIO_InitStruct.Pin = OSC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(OSC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : STAT1_Pin STAT2_Pin */
  GPIO_InitStruct.Pin = STAT1_Pin|STAT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_A_Pin BTN_B_Pin */
  GPIO_InitStruct.Pin = BTN_A_Pin|BTN_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/*void calibrate(){ //Find notes on how this works
	calibrationA = 1201200000 - (int32_t(	(double(calibration)/20.0) * 1001));
	calibrationArray[0] = 1201200000 - (int32_t(	(double(calibration)/20.0) * 1001));
	calibrationArray[1] = 1200000000 - (int32_t(	(double(calibration)/20.0) * 1000));
	calibrationArray[2] = 1200000000 - (int32_t(	(double(calibration)/20.0) * 1000));
	calibrationArray[3] = 1201200000 - (int32_t(	(double(calibration)/20.0) * 1001));
	calibrationArray[4] = 1200000000 - (int32_t(	(double(calibration)/20.0) * 1000));
}*/


void mcStop(){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  // GPIO Ports Clock Enable 
//__HAL_RCC_GPIOA_CLK_ENABLE();

//Configure GPIO pin : PA10 
GPIO_InitStruct.Pin = GPIO_PIN_10;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
GPIO_InitStruct.Pull = GPIO_NOPULL;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//Configure GPIO pin : PA9 
GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_9;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
GPIO_InitStruct.Pull = GPIO_NOPULL;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//Configure GPIO pin : PA0 
GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_0;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
GPIO_InitStruct.Pull = GPIO_PULLDOWN;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0); 
HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 0); 
HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 0); 
 HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 
 HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
 HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); 

HAL_SuspendTick();


HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
//HAL_ResumeTick();
//SystemClock_Config();
//HAL_ResumeTick();
//MX_GPIO_Init();
//HAL_NVIC_SystemReset();
}

/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  powerOff = false;
  HAL_PWR_EnterSTANDBYMode();
    SystemClock_Config ();
	  HAL_ResumeTick();
    //HAL_NVIC_SystemReset();
    MX_GPIO_Init();
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    //updateDisplay(0x1);
    //powerCountdown = 10;
    HAL_NVIC_SystemReset();
  
}*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    //if ( GPIO_Pin == GPIO_PIN_8)
	if (htim->Instance == TIM2)
    {
//Offset++;
		__disable_irq();
/* 
    Credit for this loop to read out the timecode:
    https://forum.arduino.cc/t/smpte-jam-sync/529740/4
*/
    	edgeTimeDiff = __HAL_TIM_GetCounter(htim);          // Get time difference between this and last edge

    	__HAL_TIM_SetCounter(htim,0);
    	                                    // Store time of this edge



    	  if ((edgeTimeDiff < uMin1) or (edgeTimeDiff > uMax0)) { // Drop out now if edge time not withing bounds
    	    bitSet(tcFlags, tcFrameError);
    	    jamCount = 0;
    	    __enable_irq();
    	    //Offset++;
    	    return;
    	  }

    	  if (edgeTimeDiff > uMax1)                               // A zero bit arrived
    	  {
    	    if (bitRead(tcFlags, tcHalfOne) == 1){                // But we are expecting a 1 edge
    	      bitClear(tcFlags, tcHalfOne);
    	      clearBuffer(tc, sizeof(tc)); //TODO WHAT DOES?
    	      __enable_irq();
    	      return;
    	    }
    	    // 0 bit
    	    shiftRight(tc, sizeof(tc));                           // Rotate buffer right
    	    // Shift replaces top bit with zero so nothing else to do
    	    //bitClear(tc[0], 7);                                   // Reset the 1 bit in the buffer
    	  }
    	  else                                                    // Not zero so must be a 1 bit
    	  { // 1 bit
    	    if (bitRead(tcFlags, tcHalfOne) == 0){                // First edge of a 1 bit
    	      bitSet(tcFlags, tcHalfOne);                         // Flag we have the first half
    	      __enable_irq();
    	      return;
    	    }
    	    // Second edge of a 1 bit
    	    bitClear(tcFlags, tcHalfOne);                         // Clear half 1 flag
    	    shiftRight(tc, sizeof(tc));                           // Rotate buffer right
    	    bitSet(tc[0], 7);                                     // Set the 1 bit in the buffer
    	  }
    	  // Congratulations, we have managed to read a valid 0 or 1 bit into buffer
    	  if (uint16_t( (tc[0] << 8) | (tc[1] & 0xff) ) == sync){                        // Last 2 bytes read = sync?
    	    bitClear(tcFlags, tcFrameError);                      // Clear framing error
    	    bitClear(tcFlags, tcOverrun);                         // Clear overrun error
    	    if (bitRead(tcFlags, tcForceUpdate) == 1){
    	      bitClear(tcFlags, tcValid);                         // Signal last TC read
    	    }
    	    if (bitRead(tcFlags, tcValid) == 1){                  // Last TC not read
    	      bitSet(tcFlags, tcOverrun);                         // Flag overrun error
    	     // __enable_irq();
    	     // return;                                             // Do nothing else
    	    }
    	    //if (jamEnable){
    	    	for (uint8_t x = 0; x < sizeof(tcIN); x++){            // Copy buffer without sync word
    	    		tcIN[x] = tc[x + 2];
    	    	}
    	    //}
    	    bitSet(tcFlags, tcValid);                             // Signal valid TC
    	    jamCount++;
    	    if (jamCount > 23){
    	    	//jamEnable = false;
    	    	jamCount = 0;
    	    	tcJammed = true;
initTimecode();
    	    	//HAL_TIM_Base_Stop_IT(&htim16);
    	    //endBit = false;
    	    //bitNumber = 0;
    	    //byteNumber = 0;
    	    //writeState = 0x00;
			//__HAL_TIM_SetCounter(&htim16,1500);
    	    //HAL_TIM_Base_Start_IT(&htim16);

    	    	//HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_3);
    	    }
    	    __enable_irq();

    	    //uSeconds = 260;
    	    //if (tcTimer) {
    	    		//tcTimer = false;
    	    //initTimecode();
    	    		//HAL_TIM_Base_Start_IT(&htim2);
    	    //}
    	  }

    }
	__enable_irq();
}

void clearBuffer(uint8_t theArray[], uint8_t theArraySize){
  for (uint8_t x = 0; x < theArraySize - 1; x++){
    theArray[x] = 0;
  }
}

void shiftRight(uint8_t theArray[], uint8_t theArraySize){
  uint8_t x;
  for (x = theArraySize; x > 0; x--){
    uint8_t xBit = bitRead(theArray[x - 1], 0);
    theArray[x] = theArray[x] >> 1;
    theArray[x] = theArray[x] | (xBit << 7);
  }
  theArray[x] = theArray[x] >> 1;
}




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*if (htim == &htim6){
		if (( (GPIOB->ODR >> 12) & 0x01) == 1){
			  (GPIOB->ODR) &= ~(1UL << (12));
		  } else {
			  if (ledCount > 10){
				  (GPIOB->ODR) |= (1UL << (12));
				  ledCount = 0;
			  }
		  }
		ledCount++;
	}*/
  // Check which version of the timer triggered this callback and toggle LED
  if (htim == &htim16 )
  {
	  switch (writeState){
	  case 0x00:

		  if (( (GPIOB->ODR >> 1) & 0x01) == 1){
			  (GPIOB->ODR) &= ~(1UL << (1));
		  } else {
			  (GPIOB->ODR) |= (1UL << (1));
		  }

		  if (((tcWriteBuf[byteNumber] >> bitNumber) & 0x01) == 1){
			  writeState = 0x01;
		  } else {
			  writeState = 0x02;
		  }
		  bitNumber++;
		  if (bitNumber == 8){
			  bitNumber = 0;
			  byteNumber++;
			  if (byteNumber == 10){
				  byteNumber = 0;

				  for (int i=0; i<10; i++){
			  tcWriteBuf[i] = tcWrite[i];
		  }
			  }
		  }
		  break;

	  case 0x01:
		  if (( (GPIOB->ODR >> 1) & 0x01) == 1){
			  (GPIOB->ODR) &= ~(1UL << (1));
		  } else {
			  (GPIOB->ODR) |= (1UL << (1));
		  }
		  writeState = 0x00;
		  break;
	  case 0x02:
		  writeState = 0x00;
		  break;
	  case 0x03:
		  writeState = 0x04;
		  break;
	  case 0x04:
		  writeState = 0x01;
		  break;
	  case 0x05:
		  writeState = 0x06;

		  break;
	  case 0x06:
		  writeState = 0x07;
		  if (( (GPIOB->ODR >> 1) & 0x01) == 1){
			  (GPIOB->ODR) &= ~(1UL << (1));
		  } else {
			  (GPIOB->ODR) |= (1UL << (1));
		  }
		  break;
	  case 0x07:
		  writeState = 0x01;
		  break;
	  }

  }
  if (htim == &htim7){  //Triggers once per frame
	  __disable_irq();
	  clockFrame++;
	  if (compensate){    //If coming back from compensation
		  //reset to proper ARR
		  __HAL_TIM_SET_AUTORELOAD(&htim7,frameRateARR[frameRate]);
		  //TIM7->ARR = 50049;
		  compensate = false;
		  compensationCounter = 0;
	  }
    //If it's time to compensate
	  if (compensationCounter == calibrationInterval[frameRate]){
		  __HAL_TIM_SET_AUTORELOAD(&htim7, (frameRateARR[frameRate] + calibrationArray[frameRate]));
		  compensate = true;

	  }
compensationCounter++;

    if (clockFrame % frameRateDivisor[frameRate] == 0){
		  (GPIOB->ODR) |= (1UL << (12));
	  } else {
			  (GPIOB->ODR) &= ~(1UL << (12));
	  }

	  __enable_irq();
  }
}

/*
void updateDisplay(uint8_t state){
	switch (state)
	{
	case 0x0:	//Blank
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen(&hi2c2);
		break;
	case 0x01:	//Standard
		tcDisplay[0] = foo[hr / 10];
			    tcDisplay[1] = foo[hr % 10];
			    tcDisplay[2] = ':';
			    tcDisplay[3] = foo[mn / 10];
			    tcDisplay[4] = foo[mn % 10];
			    tcDisplay[5] = ':';
			    tcDisplay[6] = foo[sc / 10];
			    tcDisplay[7] = foo[sc % 10];
			    tcDisplay[8] = ':';
			    tcDisplay[9] = foo[fr / 10];
			    tcDisplay[10] = foo[fr % 10];
			    if (Offset < 0){
			   dispOffset[0] = '-';
		   } else {
			   dispOffset[0] = '+';
		   }
		   if (Offset < 0){
			   dispOffset[1] = foo[(Offset * -1)/10];
			   dispOffset[2] = foo[(Offset * -1)%10];
		   } else {
			   dispOffset[1] = foo[Offset/10];
			   dispOffset[2] = foo[Offset%10];
		   }
	//ssd1306_SetCursor(44, 20);
		      //ssd1306_WriteString("Offset", Font_7x10, White);
		      //ssd1306_SetCursor(48,32);
		      //ssd1306_WriteString(dispOffset, Font_11x18, White);
		      //ssd1306_SetCursor(4,32);
		      //ssd1306_WriteString("<", Font_11x18, White);
		      //ssd1306_SetCursor(116,32);
		      //ssd1306_WriteString(">", Font_11x18, White);

		      ssd1306_SetCursor(4,24);
		      ssd1306_WriteString(tcDisplay,Font_11x18,White);

		      ssd1306_SetCursor(4,6);
		      ssd1306_WriteString("23.98",Font_7x10,White);

		      ssd1306_SetCursor(92,52);
		      ssd1306_WriteString("Menu",Font_7x10,White);


		      ssd1306_DrawBattery(battStatus);
		      ssd1306_SetCursor(34,52);
		      battPrint = int(battStatus * 100.0);
		      if (battPrint == 100){
		    	  battDisp[0] = '1';
		    	  battDisp[1] = '0';
		    	  battDisp[2] = '0';
		    	  battDisp[3] = '%';
		      }else {
		    	  battDisp[0] = ' ';
		    	  battDisp[1] = foo[battPrint / 10];
		    	  battDisp[2] = foo[battPrint % 10];
		    	  if (!stat1 && stat2) battDisp[3] = '^';
		    	  else if (stat1 && !stat2) battDisp[3] = '!';
		    	  else battDisp[3] = '%';
		      }
		      battDisp[4] = ' ';
		      ssd1306_WriteString(battDisp,Font_7x10,White);
		      ssd1306_UpdateScreen(&hi2c2);
		break;
	case 0x02:	//Locked, button bumped
		tcDisplay[0] = foo[hr / 10];
			    tcDisplay[1] = foo[hr % 10];
			    tcDisplay[2] = ':';
			    tcDisplay[3] = foo[mn / 10];
			    tcDisplay[4] = foo[mn % 10];
			    tcDisplay[5] = ':';
			    tcDisplay[6] = foo[sc / 10];
			    tcDisplay[7] = foo[sc % 10];
			    tcDisplay[8] = ':';
			    tcDisplay[9] = foo[fr / 10];
			    tcDisplay[10] = foo[fr % 10];
		ssd1306_Fill(Black);
		ssd1306_SetCursor(44,20);
		ssd1306_WriteString("Locked", Font_7x10, White);
		ssd1306_SetCursor(24,30);
		ssd1306_WriteString("Hold < and >", Font_7x10, White);

		 ssd1306_SetCursor(34,40);
		ssd1306_WriteString("to unlock", Font_7x10, White);
		ssd1306_SetCursor(2,6);
		      ssd1306_WriteString(tcDisplay,Font_7x10,White);

		      ssd1306_SetCursor(90,6);
		      ssd1306_WriteString("23.98",Font_7x10,White);

		      ssd1306_DrawBattery(battStatus);
		      ssd1306_SetCursor(34,52);
		      battPrint = int(battStatus * 100.0);
		      if (battPrint == 100){
		    	  battDisp[0] = '1';
		    	  battDisp[1] = '0';
		    	  battDisp[2] = '0';
		    	  battDisp[3] = '%';
		      }else {
		    	  battDisp[0] = ' ';
		    	  battDisp[1] = foo[battPrint / 10];
		    	  battDisp[2] = foo[battPrint % 10];
		    	  if (!stat1 && stat2) battDisp[3] = '^';
		    	  else if (stat1 && !stat2) battDisp[3] = '!';
		    	  else battDisp[3] = '%';
		      }
		      battDisp[4] = ' ';
		      ssd1306_WriteString(battDisp,Font_7x10,White);
		ssd1306_UpdateScreen(&hi2c2);
		break;
	case 0x03:	//Locked, button held
		tcDisplay[0] = foo[hr / 10];
			    tcDisplay[1] = foo[hr % 10];
			    tcDisplay[2] = ':';
			    tcDisplay[3] = foo[mn / 10];
			    tcDisplay[4] = foo[mn % 10];
			    tcDisplay[5] = ':';
			    tcDisplay[6] = foo[sc / 10];
			    tcDisplay[7] = foo[sc % 10];
			    tcDisplay[8] = ':';
			    tcDisplay[9] = foo[fr / 10];
			    tcDisplay[10] = foo[fr % 10];
		ssd1306_Fill(Black);
		ssd1306_SetCursor(44,20);
		ssd1306_WriteString("Locked", Font_7x10, White);
		ssd1306_SetCursor(60,32);
		//lockDisplay = foo[lockCountdown];
		ssd1306_WriteChar(lockDisplay, Font_7x10, White);
		ssd1306_SetCursor(2,6);
		      ssd1306_WriteString(tcDisplay,Font_7x10,White);

		      ssd1306_SetCursor(90,6);
		      ssd1306_WriteString("23.98",Font_7x10,White);
		      ssd1306_DrawBattery(battStatus);
		      ssd1306_SetCursor(34,52);
		      battPrint = int(battStatus * 100.0);
		      if (battPrint == 100){
		    	  battDisp[0] = '1';
		    	  battDisp[1] = '0';
		    	  battDisp[2] = '0';
		    	  battDisp[3] = '%';
		      }else {
		    	  battDisp[0] = ' ';
		    	  battDisp[1] = foo[battPrint / 10];
		    	  battDisp[2] = foo[battPrint % 10];
		    	  if (!stat1 && stat2) battDisp[3] = '^';
		    	  else if (stat1 && !stat2) battDisp[3] = '!';
		    	  else battDisp[3] = '%';
		      }
		      battDisp[4] = ' ';
		      ssd1306_WriteString(battDisp,Font_7x10,White);
		ssd1306_UpdateScreen(&hi2c2);
		break;
	case 0x04:	//Power off
		ssd1306_Fill(Black);
		ssd1306_SetCursor(14,10);
		ssd1306_WriteString("Power Off", Font_11x18, White);
		ssd1306_SetCursor(60,36);
		//powerDisplay = foo[powerCountdown];
		ssd1306_WriteChar(powerDisplay, Font_11x18, White);
		ssd1306_UpdateScreen(&hi2c2);
		break;
	case 0x05:	//Menu
		ssd1306_Fill(Black);
		//Frame rate
		//Auto power off
		//Exit
		ssd1306_SetCursor(50,2);
		ssd1306_WriteString("Menu", Font_7x10, White);
		ssd1306_SetCursor(10,14);
		ssd1306_WriteString("Rate: ",Font_7x10, White);
		ssd1306_SetCursor(10,24);
		ssd1306_WriteString("Auto-Off: ",Font_7x10, White);
		ssd1306_SetCursor(50,50);
		ssd1306_WriteString("Exit",Font_7x10, White);

		ssd1306_SetCursor(50,14);
		switch (frameRate){
		case 0:
			ssd1306_WriteString("23.98",Font_7x10, White);
			break;
		case 1:
			ssd1306_WriteString("24",Font_7x10, White);
			break;
		case 2:
			ssd1306_WriteString("25",Font_7x10, White);
			break;
		case 3:
			ssd1306_WriteString("29.97",Font_7x10, White);
			break;
		case 4:
			ssd1306_WriteString("29.97 DF",Font_7x10, White);
			break;
		case 5:
			ssd1306_WriteString("30",Font_7x10, White);
			break;
		}
		ssd1306_SetCursor(75,24);
		switch (autoOff){
		case 0:
			ssd1306_WriteString("1 hr",Font_7x10, White);
			break;
		case 1:
			ssd1306_WriteString("2 hr",Font_7x10, White);
			break;
		case 2:
			ssd1306_WriteString("4 hr",Font_7x10, White);
			break;
		case 3:
			ssd1306_WriteString("8 hr",Font_7x10, White);
			break;
		case 4:
			ssd1306_WriteString("12 hr",Font_7x10, White);
			break;
		}

			switch (menuItem){
			case 0:
				if (menuItemSelect){
					ssd1306_SetCursor(120,14);
					ssd1306_WriteString("<",Font_7x10, White);
				}else {
					ssd1306_SetCursor(2,14);
					ssd1306_WriteString(">",Font_7x10, White);
				}
				break;
			case 1:
				if (menuItemSelect){
					ssd1306_SetCursor(120,24);
					ssd1306_WriteString("<",Font_7x10, White);
				} else {
					ssd1306_SetCursor(2,24);
					ssd1306_WriteString(">",Font_7x10, White);
				}

				break;
			case 2:
				ssd1306_SetCursor(40,50);
				ssd1306_WriteString(">",Font_7x10, White);
				ssd1306_SetCursor(79,50);
				ssd1306_WriteString("<",Font_7x10, White);
				break;
			}

		ssd1306_UpdateScreen(&hi2c2);

		break;
	}
}
*/
uint8_t  read_eeprom_reg(uint16_t addr)
{
        uint8_t buffer[] = {addr};
        uint8_t value;
        while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
        {
        }
        while (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(0x50<<1), 3, 100) != HAL_OK) { }
        while (HAL_I2C_Master_Transmit(&hi2c1,                // i2c handle
                                      (uint16_t)(0x50<<1),    // i2c address, left aligned
                                      (uint8_t *)&buffer,    // data to send
                                      2,                    // how many bytes - 16 bit register address = 2 bytes
                                      100)                    // timeout
                                      != HAL_OK)            // result code
        {
            if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
            {
                Error_Handler();
            }
        }
        while (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(0x50<<1), 3, 100) != HAL_OK) {}
        while (HAL_I2C_Master_Receive(&hi2c1,            // i2c handle
                                     (uint16_t)(0x50<<1),    // i2c address, left aligned
                                     (uint8_t *)&value,        // pointer to address of where to store received data
                                     1,                        // expecting one byte to be returned
                                     100)                    // timeout
                                     != HAL_OK)                // result code
        {
            if (HAL_I2C_GetError (&hi2c1) != HAL_I2C_ERROR_AF)
            {
                Error_Handler();
            }
        }
return value;
}


void write_eeprom_reg(uint16_t reg_addr, uint8_t value)
{
    uint8_t d[3];
    d[0] = (reg_addr >> 8) & 0xFF;                        // high byte of 16-bit register address
    d[1] = (reg_addr) & 0xFF;                            // low byte of 16-bit register address
    d[2] = value;                                        // what value are we writing to the register
    while (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(0x50<<1), 3, 100) != HAL_OK) {}
    if (HAL_I2C_Master_Transmit(&hi2c1,                    // i2c handle
                               (uint16_t)(0x50<<1),     // i2c address, left aligned
                               (uint8_t *)d,             // pointer to the buffer containing the data
                               sizeof(d),                 // how many bytes are we transmitting
                               1000)                    // timeout value
                               != HAL_OK)                // result code
    {
        Error_Handler();
    } else {
    }
}
bool readEEPROM(){
  bool calibrationReadOK = true;
  uint8_t readCal[4];
  uint8_t rateAutoOffRead;
  /*if(HAL_I2C_Mem_Read(memI2C, 0x50<<1, 0x0001, 1, &frameRate, 1, 1000)!= HAL_OK)	//offset
{
	//memOffset = 24;
	
}*/
HAL_Delay(20);
if(HAL_I2C_Mem_Read(memI2C, 0x50<<1, 0x0002, 1, &intOffset, 1, 1000)!= HAL_OK)	//frame rate
{
	//frameRate = 0;
	
}
if(HAL_I2C_Mem_Read(memI2C, 0x50<<1, 0x0001, 1, &rateAutoOffRead, 1, 1000)!= HAL_OK)	//frame rate
{
	//frameRate = 0;
	
}
HAL_Delay(20);
/*if(HAL_I2C_Mem_Read(memI2C, 0x50<<1, 0x0003, 1, &autoOff, 1, 1000)!= HAL_OK)	//auto off
{
	//autoOff = 0;
	
}
HAL_Delay(20);*/
frameRate = rateAutoOffRead & 0x0F;
autoOff = (rateAutoOffRead & 0xF0) >> 4;
for (int i=0; i<4; i++){
if(HAL_I2C_Mem_Read(memI2C, 0x50<<1, 0x0003+i, 1, &readCal[i], 1, 1000)!= HAL_OK)	//auto off
{
	calibrationReadOK = false;
  break;
}
HAL_Delay(10);
}

if (calibrationReadOK){
calibration = (readCal[0]) | (readCal[1] << 8) | (readCal[2] << 16) | (readCal[3] << 24);
if (calibration < 23000000 || calibration > 25000000){
  calibrationReadOK = false;
  calibration = 24000000;
}
}

return calibrationReadOK;
}
bool updateEEPROM(){
bool writeOK = true;
uint8_t rateAutoOffWrite;
rateAutoOffWrite = (autoOff << 4) + (frameRate);
  //Frame rate
  //Offset
  //Timeout
  //User bits

  //Calibration
  HAL_Delay(10);
	/*if(HAL_I2C_Mem_Write(memI2C , 0x50<<1, 0x0001, 1, &frameRate, 1,1000)!= HAL_OK)	//offset
				{
						  writeOK = false;
				}
				  HAL_Delay(10);*/
          if(HAL_I2C_Mem_Write(memI2C , 0x50<<1, 0x0001, 1, &rateAutoOffWrite, 1,1000)!= HAL_OK)	//offset
				{
						  writeOK = false;
				}
				  HAL_Delay(10);
			  
			 
				  if(HAL_I2C_Mem_Write(memI2C , 0x50<<1, 0x0002, 1, &intOffset, 1,250)!= HAL_OK)	//framerate
					  {
						  writeOK = false;
					  }
				  HAL_Delay(10);
			  
			  
				  /*if(HAL_I2C_Mem_Write(memI2C , 0x50<<1, 0x0003, 1, &autoOff, 1,250)!= HAL_OK)	//autooff
					  {
						  writeOK = false;
					  }
				  HAL_Delay(10);*/
return writeOK;
			  
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
