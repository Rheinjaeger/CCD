/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32l0xx_hal.h"

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
#define PreAmp_Out_Pin GPIO_PIN_0
#define PreAmp_Out_GPIO_Port GPIOA
#define ADC_Select_Pin GPIO_PIN_1
#define ADC_Select_GPIO_Port GPIOA
#define Temp_2_Pin GPIO_PIN_3
#define Temp_2_GPIO_Port GPIOA
#define Ethernet_CE_Pin GPIO_PIN_4
#define Ethernet_CE_GPIO_Port GPIOA
#define Ethernet_SCLK_Pin GPIO_PIN_5
#define Ethernet_SCLK_GPIO_Port GPIOA
#define Ethernet_MISO_Pin GPIO_PIN_6
#define Ethernet_MISO_GPIO_Port GPIOA
#define Ethernet_MOSI_Pin GPIO_PIN_7
#define Ethernet_MOSI_GPIO_Port GPIOA
#define I1_Pin GPIO_PIN_0
#define I1_GPIO_Port GPIOB
#define I2_Pin GPIO_PIN_1
#define I2_GPIO_Port GPIOB
#define I3_Pin GPIO_PIN_2
#define I3_GPIO_Port GPIOB
#define Ethernet_RSTn_Pin GPIO_PIN_10
#define Ethernet_RSTn_GPIO_Port GPIOB
#define __WP_Pin GPIO_PIN_11
#define __WP_GPIO_Port GPIOB
#define Flash_CS_Pin GPIO_PIN_12
#define Flash_CS_GPIO_Port GPIOB
#define Flash_SCK_Pin GPIO_PIN_13
#define Flash_SCK_GPIO_Port GPIOB
#define Flash_MISO_Pin GPIO_PIN_14
#define Flash_MISO_GPIO_Port GPIOB
#define Flash_MOSI_Pin GPIO_PIN_15
#define Flash_MOSI_GPIO_Port GPIOB
#define Ethernet_INTn_Pin GPIO_PIN_8
#define Ethernet_INTn_GPIO_Port GPIOA
#define Digipot_SHDN__Pin GPIO_PIN_11
#define Digipot_SHDN__GPIO_Port GPIOA
#define CD_Gate_Pin GPIO_PIN_12
#define CD_Gate_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_3
#define R1_GPIO_Port GPIOB
#define R2_Pin GPIO_PIN_4
#define R2_GPIO_Port GPIOB
#define R3_Pin GPIO_PIN_5
#define R3_GPIO_Port GPIOB
#define R_Pin GPIO_PIN_6
#define R_GPIO_Port GPIOB
#define E_PM2_Pin GPIO_PIN_7
#define E_PM2_GPIO_Port GPIOB
#define E_PM1_Pin GPIO_PIN_8
#define E_PM1_GPIO_Port GPIOB
#define E_PM0_Pin GPIO_PIN_9
#define E_PM0_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
