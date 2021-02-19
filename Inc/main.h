/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MODE_Pin GPIO_PIN_3
#define MODE_GPIO_Port GPIOC
#define HOME_Pin GPIO_PIN_0
#define HOME_GPIO_Port GPIOA
#define CSOUT_Pin GPIO_PIN_1
#define CSOUT_GPIO_Port GPIOA
#define DIR_Pin GPIO_PIN_5
#define DIR_GPIO_Port GPIOA
#define BRAKE_Pin GPIO_PIN_6
#define BRAKE_GPIO_Port GPIOA
#define FF1_Pin GPIO_PIN_4
#define FF1_GPIO_Port GPIOC
#define FF2_Pin GPIO_PIN_5
#define FF2_GPIO_Port GPIOC
#define RESET_Pin GPIO_PIN_0
#define RESET_GPIO_Port GPIOB
#define COAST_Pin GPIO_PIN_1
#define COAST_GPIO_Port GPIOB
#define DIRO_Pin GPIO_PIN_12
#define DIRO_GPIO_Port GPIOB
#define ESF_Pin GPIO_PIN_13
#define ESF_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_14
#define LED_GPIO_Port GPIOB
#define USB_PWR_Pin GPIO_PIN_7
#define USB_PWR_GPIO_Port GPIOC
#define LCD_PWR_Pin GPIO_PIN_8
#define LCD_PWR_GPIO_Port GPIOC
#define PWM_Pin GPIO_PIN_8
#define PWM_GPIO_Port GPIOA
#define STOP_SW_Pin GPIO_PIN_10
#define STOP_SW_GPIO_Port GPIOC
#define LED_B_Pin GPIO_PIN_11
#define LED_B_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_12
#define LED_G_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_2
#define LED_R_GPIO_Port GPIOD
#define SPI3_CS_Pin GPIO_PIN_6
#define SPI3_CS_GPIO_Port GPIOB
#define WP_Pin GPIO_PIN_8
#define WP_GPIO_Port GPIOB
#define HOLD_Pin GPIO_PIN_9
#define HOLD_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
