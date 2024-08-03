/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define SPI4_CS_Pin GPIO_PIN_4
#define SPI4_CS_GPIO_Port GPIOE
#define GND1_Pin GPIO_PIN_0
#define GND1_GPIO_Port GPIOC
#define I_Phase_U_Pin GPIO_PIN_1
#define I_Phase_U_GPIO_Port GPIOC
#define U_Bat_Pin GPIO_PIN_2
#define U_Bat_GPIO_Port GPIOC
#define TEMP_MOS_Pin GPIO_PIN_1
#define TEMP_MOS_GPIO_Port GPIOA
#define I_Phase_W_Pin GPIO_PIN_2
#define I_Phase_W_GPIO_Port GPIOA
#define I_Phase_V_Pin GPIO_PIN_3
#define I_Phase_V_GPIO_Port GPIOA
#define U_Phase_U_Pin GPIO_PIN_4
#define U_Phase_U_GPIO_Port GPIOA
#define DRIVER_FAULT_Pin GPIO_PIN_6
#define DRIVER_FAULT_GPIO_Port GPIOA
#define U_Phase_V_Pin GPIO_PIN_5
#define U_Phase_V_GPIO_Port GPIOC
#define U_Phase_W_Pin GPIO_PIN_0
#define U_Phase_W_GPIO_Port GPIOB
#define GND2_Pin GPIO_PIN_1
#define GND2_GPIO_Port GPIOB
#define DRIVER_NBRAKE_Pin GPIO_PIN_13
#define DRIVER_NBRAKE_GPIO_Port GPIOE
#define DRIVER_WARN_Pin GPIO_PIN_14
#define DRIVER_WARN_GPIO_Port GPIOE
#define DRIVER_ENABLE_Pin GPIO_PIN_12
#define DRIVER_ENABLE_GPIO_Port GPIOD
#define SPI6_CS_Pin GPIO_PIN_7
#define SPI6_CS_GPIO_Port GPIOD
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_Port GPIOB
#define Z_DRIVER_SLEEP_Pin GPIO_PIN_8
#define Z_DRIVER_SLEEP_GPIO_Port GPIOB
#define Z_DRIVER_ENABLE_Pin GPIO_PIN_9
#define Z_DRIVER_ENABLE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
