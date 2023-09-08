/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define MATRIX_DISPLAY_UNIT1  0
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUZZER_Pin GPIO_PIN_1
#define BUZZER_GPIO_Port GPIOA
#define SS_SD_Pin GPIO_PIN_3
#define SS_SD_GPIO_Port GPIOA
#define SS_LED_Pin GPIO_PIN_4
#define SS_LED_GPIO_Port GPIOA
#define JOYSTICK_2_DOWN_Pin GPIO_PIN_12
#define JOYSTICK_2_DOWN_GPIO_Port GPIOB
#define JOYSTICK_2_UP_Pin GPIO_PIN_13
#define JOYSTICK_2_UP_GPIO_Port GPIOB
#define JOYSTICK_2_LEFT_Pin GPIO_PIN_14
#define JOYSTICK_2_LEFT_GPIO_Port GPIOB
#define JOYSTICK_2_RIGHT_Pin GPIO_PIN_15
#define JOYSTICK_2_RIGHT_GPIO_Port GPIOB
#define JOYSTICK_1_RIGHT_Pin GPIO_PIN_8
#define JOYSTICK_1_RIGHT_GPIO_Port GPIOA
#define JOYSTICK_1_LEFT_Pin GPIO_PIN_9
#define JOYSTICK_1_LEFT_GPIO_Port GPIOA
#define JOYSTICK_1_UP_Pin GPIO_PIN_10
#define JOYSTICK_1_UP_GPIO_Port GPIOA
#define JOYSTICK_1_DOWN_Pin GPIO_PIN_11
#define JOYSTICK_1_DOWN_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
