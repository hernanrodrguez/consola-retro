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

#include "DOT_MATRIX.h"
#include "lcd1602_i2c.h"
#include "joystick.h"
#include "menu.h"
#include "pong.h"
#include "buzzer.h"
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

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

QueueHandle_t joysticks_queue;
QueueHandle_t game_queue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
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

		STATE_GAME_0_MENU_SHOW,
		STATE_GAME_0_MENU_HANDLE,
		STATE_GAME_0_PLAY,
		STATE_GAME_0_RULES_SHOW,
		STATE_GAME_0_RULES_HANDLE,
		STATE_GAME_0_RECORDS_SHOW,
		STATE_GAME_0_RECORDS_HANDLE,

		STATE_GAME_1_MENU_SHOW,
		STATE_GAME_1_MENU_HANDLE,
		STATE_GAME_1_PLAY,
		STATE_GAME_1_RULES_SHOW,
		STATE_GAME_1_RULES_HANDLE,
		STATE_GAME_1_RECORDS_SHOW,
		STATE_GAME_1_RECORDS_HANDLE,

		STATE_GAME_2_MENU_SHOW,
		STATE_GAME_2_MENU_HANDLE,
		STATE_GAME_2_PLAY,
		STATE_GAME_2_RULES_SHOW,
		STATE_GAME_2_RULES_HANDLE,
		STATE_GAME_2_RECORDS_SHOW,
		STATE_GAME_2_RECORDS_HANDLE,

		STATE_GAME_3_MENU_SHOW,
		STATE_GAME_3_MENU_HANDLE,
		STATE_GAME_3_PLAY,
		STATE_GAME_3_RULES_SHOW,
		STATE_GAME_3_RULES_HANDLE,
		STATE_GAME_3_RECORDS_SHOW,
		STATE_GAME_3_RECORDS_HANDLE,

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
				delay_ticks = 2000;
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
				next_state = STATE_GAME_0_MENU_SHOW;
				break;
			case 2:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_1_MENU_SHOW;
				break;
			case 3:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_2_MENU_SHOW;
				break;
			case 4:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_3_MENU_SHOW;
				break;
			}
			break;
		case STATE_MENU_CLEAR:
			if(lcd_progressive_clear(FOUR_LINES)){
				main_state = next_state;
			}
			break;
		case STATE_GAME_0_MENU_SHOW:
			if(lcd_progressive_print("        PONG        ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_0_MENU_HANDLE;
			}
			break;
		case STATE_GAME_0_MENU_HANDLE:
			switch(menu_game_handle()){ // por ahora vuelvo en todos los casos...
			case 1: // jugar
				main_state = STATE_GAME_0_PLAY;
				break;
			case 2: // reglas
				main_state = STATE_GAME_0_RULES_SHOW;
				break;
			case 3: // puntajes
				main_state = TEST_DIE;
				break;
			case 4: // volver
				main_state = STATE_WELCOME_CLEAR;
				break;
			}
			break;
		case STATE_GAME_0_PLAY:
			if(menu_game_play(0, "        PONG        ")){
				main_state = TEST_DIE;
			}
			break;
		case STATE_GAME_1_MENU_SHOW:
			if(lcd_progressive_print("       TETRIS       ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_1_MENU_HANDLE;
			}
			break;
		case STATE_GAME_1_MENU_HANDLE:
			switch(menu_game_handle()){
			case 1: // jugar
				main_state = STATE_GAME_1_PLAY;
				break;
			case 2: // reglas
				main_state = STATE_GAME_1_RULES_SHOW;
				break;
			case 3: // puntajes
				main_state = TEST_DIE;
				break;
			case 4: // volver
				main_state = STATE_WELCOME_CLEAR;
				break;
			}
			break;
		case STATE_GAME_1_PLAY:
			if(menu_game_play(1, "       TETRIS       ")){
				main_state = TEST_DIE;
			}
			break;
		case STATE_GAME_2_MENU_SHOW:
			if(lcd_progressive_print("       SNAKE        ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_2_MENU_HANDLE;
			}
			break;
		case STATE_GAME_2_MENU_HANDLE:
			switch(menu_game_handle()){
			case 1: // jugar
				main_state = STATE_GAME_2_PLAY;
				break;
			case 2: // reglas
				main_state = STATE_GAME_2_RULES_SHOW;
				break;
			case 3: // puntajes
				main_state = TEST_DIE;
				break;
			case 4: // volver
				main_state = STATE_WELCOME_CLEAR;
				break;
			}
			break;
		case STATE_GAME_2_PLAY:
			if(menu_game_play(2, "       SNAKE        ")){
				main_state = TEST_DIE;
			}
			break;
		case STATE_GAME_3_MENU_SHOW:
			if(lcd_progressive_print("       SPACE        ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_3_MENU_HANDLE;
			}
			break;
		case STATE_GAME_3_MENU_HANDLE:
			switch(menu_game_handle()){
			case 1: // jugar
				main_state = STATE_GAME_3_PLAY;
				break;
			case 2: // reglas
				main_state = STATE_GAME_3_RULES_SHOW;
				break;
			case 3: // puntajes
				main_state = TEST_DIE;
				break;
			case 4: // volver
				main_state = STATE_WELCOME_CLEAR;
				break;
			}
			break;
		case STATE_GAME_3_PLAY:
			if(menu_game_play(3, "       SPACE        ")){
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

static void test_task(void *pvParameters){

	//char MSG[] = "Hola mundo MAX7219 ";

	DOT_MATRIX_Init(&hspi1);

	while(1) {

		for(uint8_t i=0; i<32; i++){
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 0, i, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 31, i, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 0, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 31, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, i, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 31-i, i, 1);
		}

		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		//MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
		vTaskDelay(500/portTICK_PERIOD_MS);

		for(uint8_t i=0; i<32; i++){
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 0, i, 0);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 31, i, 0);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 0, 0);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 31, 0);
		}

		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
		vTaskDelay(500/portTICK_PERIOD_MS);

		for(uint8_t i=0; i<32; i++){
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 0, i, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 31, i, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 0, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 31, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 15, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, 16, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 15, i, 1);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 16, i, 1);
		}

		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
		vTaskDelay(500/portTICK_PERIOD_MS);
	}



	//pong_init();
	//pong_print_board();

/*
	while(1){

		if(pong_play()){
			vTaskSuspend(NULL);
		}

	}
	while(1){
		//buzzer_play_melody(victoryMelody6, (sizeof(victoryMelody6)/sizeof(note_t)));
		buzzer_test_melody();
		vTaskDelay(500/portTICK_PERIOD_MS);
	}*/
}

void two_player_game_task(void *pvParameters){

	typedef enum{
		STATE_PLAYING,
		STATE_PAUSE
	}game_status_t;

	static game_status_t game_status = STATE_PLAYING;

	uint8_t game_data;
	uint8_t joystick;

	while(1){

		switch(game_status){
		case STATE_PLAYING:
			if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
				if(joystick == JOYSTICK_1_UP){
					game_data = PLAYER_1_POINT;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				}
				if(joystick == JOYSTICK_2_UP){
					game_data = PLAYER_2_POINT;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				}
				if(joystick == JOYSTICK_1_RIGHT){
					game_data = PLAYER_1_WIN;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					vTaskDelete(NULL);
				}
				if(joystick == JOYSTICK_2_RIGHT){
					game_data = PLAYER_2_WIN;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					vTaskDelete(NULL);
				}
				if(joystick == JOYSTICK_1_PULS || joystick == JOYSTICK_2_PULS){
					xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
					game_data = PLAYER_1_PAUSE; // por ahora manejo indistintamente las pausas
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					game_status = STATE_PAUSE;
					taskYIELD();
				}
			}
			break;
		case STATE_PAUSE:
			if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
				if(game_data == GAME_RESUME){
					game_status = STATE_PLAYING;
				}
				if(game_data == GAME_OVER || game_data == GAME_RESET){
					game_status = STATE_PLAYING;
					vTaskDelete(NULL);
				}
				if(game_data == PLAYER_1_PAUSE){
					game_data = PLAYER_1_PAUSE;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					taskYIELD();
				}
			}
			break;
		}
	}
}

void single_player_game_task(void *pvParameters){

	typedef enum{
		STATE_PLAYING,
		STATE_PAUSE
	}game_status_t;

	static game_status_t game_status = STATE_PLAYING;

	uint8_t game_data;
	uint8_t joystick;

	while(1){

		switch(game_status){
		case STATE_PLAYING:
			if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
				if(joystick == JOYSTICK_1_UP){
					game_data = SINGLE_PLAYER_POINT;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				}
				if(joystick == JOYSTICK_2_UP){
					game_data = SINGLE_PLAYER_LIVE;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				}
				if(joystick == JOYSTICK_1_RIGHT){
					game_data = SINGLE_PLAYER_WIN;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					vTaskDelete(NULL);
				}
				if(joystick == JOYSTICK_2_RIGHT){
					game_data = SINGLE_PLAYER_LOST;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					vTaskDelete(NULL);
				}
				if(joystick == JOYSTICK_1_PULS || joystick == JOYSTICK_2_PULS){
					xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
					game_data = SINGLE_PLAYER_PAUSE; // por ahora manejo indistintamente las pausas
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					game_status = STATE_PAUSE;
					taskYIELD();
				}
			}
			break;
		case STATE_PAUSE:
			if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
				if(game_data == GAME_RESUME){
					game_status = STATE_PLAYING;
				}
				if(game_data == GAME_OVER || game_data == GAME_RESET){
					game_status = STATE_PLAYING;
					vTaskDelete(NULL);
				}
				if(game_data == SINGLE_PLAYER_PAUSE){
					game_data = SINGLE_PLAYER_PAUSE;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					taskYIELD();
				}
			}
			break;
		}
	}
}

void pong_task(void *pvParameters){
	DOT_MATRIX_Init(&hspi1);
	pong_init();

	while(1){
		pong_play();
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  joysticks_queue = xQueueCreate(1, sizeof(uint8_t));
  game_queue = xQueueCreate(1, sizeof(uint8_t));

  lcd_init(&hi2c1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

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
/*
  xTaskCreate(game_task,
			  "game_task",
			  configMINIMAL_STACK_SIZE,
			  NULL,
			  1,
			  NULL);


  xTaskCreate(test_task,
			  "test_task",
			  configMINIMAL_STACK_SIZE,
			  NULL,
			  1,
			  NULL);*/

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
////
  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */
////
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
////
  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */
//////
  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */
//////
  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
//////
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 107;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 20;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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

  /*Configure GPIO pins : JOYSTICK_2_DOWN_Pin JOYSTICK_2_UP_Pin JOYSTICK_2_LEFT_Pin JOYSTICK_2_RIGHT_Pin */
  GPIO_InitStruct.Pin = JOYSTICK_2_DOWN_Pin|JOYSTICK_2_UP_Pin|JOYSTICK_2_LEFT_Pin|JOYSTICK_2_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : JOYSTICK_1_RIGHT_Pin JOYSTICK_1_LEFT_Pin JOYSTICK_1_UP_Pin JOYSTICK_1_DOWN_Pin */
  GPIO_InitStruct.Pin = JOYSTICK_1_RIGHT_Pin|JOYSTICK_1_LEFT_Pin|JOYSTICK_1_UP_Pin|JOYSTICK_1_DOWN_Pin;
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

	//MATRIX_TMR_OVF_ISR(htim);

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
