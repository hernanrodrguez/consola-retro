/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "max7219.h"
#include "lcd1602_i2c.h"
#include "joystick.h"
#include "menu.h"
#include <stdio.h>
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

QueueHandle_t joysticks_queue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void main_task(void *pvParameters){

	typedef enum {
		STATE_WELCOME_SHOW,
		STATE_WELCOME_DELAY,
		STATE_WELCOME_CLEAR,
		STATE_MENU_SHOW,
		STATE_MENU_HANDLE,
		STATE_MENU_CLEAR,
		STATE_GAME_0,
		STATE_GAME_1,
		STATE_GAME_2,
		STATE_GAME_3,
		TEST_DIE
	} main_state_t;

	static main_state_t main_state = STATE_WELCOME_SHOW;
	static main_state_t next_state;
	static uint32_t start_ticks, delay_ticks;

	while(1){
		switch(main_state){
		case STATE_WELCOME_SHOW:
			if(lcd_progressive_print("                    ",
									 " CONSOLA DE JUEGOS  ",
									 "       RETRO        ",
									 "                    ",
									 FOUR_LINES)){
				delay_ticks = 3000;
				start_ticks = HAL_GetTick();
				main_state = STATE_WELCOME_DELAY;
			}
			break;
		case STATE_WELCOME_DELAY:
			if(HAL_GetTick() - start_ticks > delay_ticks){
				main_state = STATE_WELCOME_CLEAR;
			}
			break;
		case STATE_WELCOME_CLEAR:
			if(lcd_progressive_clear(FOUR_LINES)){
				main_state = STATE_MENU_SHOW;
			}
			break;
		case STATE_MENU_SHOW:
			if(lcd_progressive_print("   MENU PRINCIPAL   ",
									 "Seleccione un juego:",
									 "  Pong      Tetris  ",
									 "  Snake     Space   ",
									 FOUR_LINES)){
				main_state = STATE_MENU_HANDLE;
			}
			break;
		case STATE_MENU_HANDLE:
			switch(menu_handle()){
			case 1:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_0;
				break;
			case 2:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_1;
				break;
			case 3:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_2;
				break;
			case 4:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_3;
				break;
			}
			break;
		case STATE_MENU_CLEAR:
				if(lcd_progressive_clear(FOUR_LINES)){
					main_state = next_state;
				}
				break;
		case STATE_GAME_0:
			if(lcd_progressive_print("                    ",
									 "    STATE GAME 0    ",
									 "                    ",
									 "                    ",
									 FOUR_LINES)){
				main_state = TEST_DIE;
			}
			break;
		case STATE_GAME_1:
			if(lcd_progressive_print("                    ",
									 "    STATE GAME 1    ",
									 "                    ",
									 "                    ",
									 FOUR_LINES)){
				main_state = TEST_DIE;
			}
			break;
		case STATE_GAME_2:
			if(lcd_progressive_print("                    ",
									 "    STATE GAME 2    ",
									 "                    ",
									 "                    ",
									 FOUR_LINES)){
				main_state = TEST_DIE;
			}
			break;
		case STATE_GAME_3:
			if(lcd_progressive_print("                    ",
									 "    STATE GAME 3    ",
									 "                    ",
									 "                    ",
									 FOUR_LINES)){
				main_state = TEST_DIE;
			}
			break;
		case TEST_DIE:
			if(lcd_progressive_clear(FOUR_LINES)){
				main_state = STATE_MENU_SHOW;
			}
			break;
		}
	}
}
/*
static void led_task(void *pvParameters){
	uint8_t matrix[8];

	matrix[0] = 0b01010101;
	matrix[1] = 0b10101010;
	matrix[2] = 0b00000000;
	matrix[3] = 0b11111111;
	matrix[4] = 0b11111111;
	matrix[5] = 0b00000000;
	matrix[6] = 0b10101010;
	matrix[7] = 0b01010101;

	max7219_init(&hspi1, SS_LED_GPIO_Port, SS_LED_Pin);

	while(1){
		max7219_print(matrix);
		for(uint32_t i=0; i<8; i++){
			matrix[i]++;
		}
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
*/
static void joysticks_task(void *pvParameters){

	uint8_t joystick;

	init_joysticks();

	while(1){
		joystick = read_joysticks();
		if(joystick != NONE){
			xQueueSendToBack(joysticks_queue, &joystick, portMAX_DELAY);
		}
	}
}

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
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  joysticks_queue = xQueueCreate(1, sizeof(uint8_t));

  lcd_init(&hi2c1);

  xTaskCreate(main_task,
			  "main_task",
			  configMINIMAL_STACK_SIZE,
			  NULL,
			  1,
			  NULL);

  xTaskCreate(joysticks_task,
			  "joysticks_task",
			  configMINIMAL_STACK_SIZE,
			  NULL,
			  1,
			  NULL);

  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BUZZER_Pin|SS_SD_Pin|SS_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BUZZER_Pin SS_SD_Pin SS_LED_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin|SS_SD_Pin|SS_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : EJE_Y_DOWN_2_Pin EJE_Y_UP_2_Pin EJE_X_DOWN_2_Pin EJE_X_UP_2_Pin */
  GPIO_InitStruct.Pin = EJE_Y_DOWN_2_Pin|EJE_Y_UP_2_Pin|EJE_X_DOWN_2_Pin|EJE_X_UP_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : EJE_X_UP_1_Pin EJE_X_DOWN_1_Pin EJE_Y_UP_1_Pin EJE_Y_DOWN_1_Pin */
  GPIO_InitStruct.Pin = EJE_X_UP_1_Pin|EJE_X_DOWN_1_Pin|EJE_Y_UP_1_Pin|EJE_Y_DOWN_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
