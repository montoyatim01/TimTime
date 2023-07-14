/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TC_IN_Pin GPIO_PIN_2
#define TC_IN_GPIO_Port GPIOA
#define BATT_CHECK_Pin GPIO_PIN_3
#define BATT_CHECK_GPIO_Port GPIOA
#define TCVC_Pin GPIO_PIN_5
#define TCVC_GPIO_Port GPIOA
#define TC_OUT_Pin GPIO_PIN_1
#define TC_OUT_GPIO_Port GPIOB
#define MON_SCL_Pin GPIO_PIN_10
#define MON_SCL_GPIO_Port GPIOB
#define MON_SDA_Pin GPIO_PIN_11
#define MON_SDA_GPIO_Port GPIOB
#define LED_STATUS_Pin GPIO_PIN_12
#define LED_STATUS_GPIO_Port GPIOB
#define OSC_Pin GPIO_PIN_8
#define OSC_GPIO_Port GPIOA
#define STAT1_Pin GPIO_PIN_9
#define STAT1_GPIO_Port GPIOA
#define STAT2_Pin GPIO_PIN_10
#define STAT2_GPIO_Port GPIOA
#define MEM_SCL_Pin GPIO_PIN_6
#define MEM_SCL_GPIO_Port GPIOB
#define MEM_SDA_Pin GPIO_PIN_7
#define MEM_SDA_GPIO_Port GPIOB
#define BTN_A_Pin GPIO_PIN_8
#define BTN_A_GPIO_Port GPIOB
#define BTN_B_Pin GPIO_PIN_9
#define BTN_B_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */



#define uMax0 651 // Any time greater than this is to big
#define uMax1 390 // Midpoint timing between a 1 and 0 bit length
#define uMin1 130 // Any time less than this is to short


#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
void SystemClock_Config(void);






/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
