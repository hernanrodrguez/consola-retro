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
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
    uint16_t frequency;
    uint16_t duration;
} note_t;


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

#define REST 0 // Valor para pausa en la melodía
#define E4 329 // Frecuencia de la nota E4 en Hz
#define G4 392 // Frecuencia de la nota G4 en Hz
#define A4 440 // Frecuencia de la nota A4 en Hz
#define B4 494 // Frecuencia de la nota B4 en Hz
#define C5 523 // Frecuencia de la nota C5 en Hz
#define D5 587 // Frecuencia de la nota D5 en Hz
#define E5 659 // Frecuencia de la nota E5 en Hz
#define F5 698 // Frecuencia de la nota F5 en Hz
#define G5 784 // Frecuencia de la nota G5 en Hz
#define A5 880 // Frecuencia de la nota A5 en Hz

note_t marioBrosMelody[] = {
    {659, 250},
    {0, 100}, // Pausa
    {659, 250},
    {0, 100}, // Pausa
    {659, 250},
    {0, 100}, // Pausa
    {523, 250},
    {659, 250},
    {0, 100}, // Pausa
    {784, 250},
    {0, 400}, // Pausa
    {392, 250},
    {0, 400}, // Pausa
    {523, 250},
    {0, 400}, // Pausa
    {392, 250},
};

note_t twinkleTwinkleMelody[] = {
    {523, 400},
    {523, 400},
    {659, 400},
    {659, 400},
    {783, 400},
    {783, 400},
    {659, 800},
    {523, 400},
    {523, 400},
    {392, 400},
    {392, 400},
    {329, 400},
    {329, 400},
    {261, 800},
};

note_t jingleBellsMelody[] = {
    {659, 250},
    {659, 250},
    {0, 250}, // Pausa
    {659, 250},
    {659, 250},
    {0, 250}, // Pausa
    {659, 250},
    {0, 250}, // Pausa
    {659, 250},
    {659, 250},
    {659, 250},
    {659, 250},
    {587, 250},
    {587, 250},
    {659, 250},
    {0, 250}, // Pausa
};

note_t happyBirthdayMelody[] = {
    {392, 400},
    {392, 400},
    {440, 800},
    {392, 400},
    {523, 800},
    {392, 400},
    {392, 400},
    {440, 800},
    {392, 400},
    {659, 800},
    {587, 400},
    {523, 400},
    {349, 800},
};

note_t harryPotterMelody[] = {
    {392, 400},
    {523, 400},
    {587, 400},
    {739, 600},
    {659, 200},
    {587, 400},
    {659, 600},
    {783, 600},
    {987, 600},
    {880, 400},
    {783, 400},
    {880, 400},
    {1046, 800},
    {987, 400},
    {880, 400},
    {783, 400},
    {659, 600},
    {587, 200},
    {659, 400},
    {987, 400},
    {880, 400},
    {783, 400},
    {659, 600},
    {587, 200},
    {523, 400},
};

note_t powerOnMelody1[] = {
    {784, 200},
    {988, 200},
    {1175, 200},
    {1397, 400},
    {1175, 200},
    {1397, 400},
    {1568, 400},
    {1760, 400},
};

note_t powerOnMelody2[] = {
    {523, 200},
    {659, 200},
    {784, 200},
    {880, 400},
    {784, 200},
    {880, 400},
    {1046, 400},
    {1175, 400},
};

note_t powerOnMelody3[] = {
    {587, 300},
    {698, 300},
    {784, 300},
    {880, 600},
    {784, 300},
    {880, 600},
    {1046, 600},
    {1175, 600},
};

note_t inGameMelody1[] = {
    {440, 300},
    {523, 300},
    {587, 300},
    {659, 600},
    {587, 300},
    {659, 600},
};

note_t inGameMelody2[] = {
    {523, 200},
    {659, 200},
    {784, 200},
    {880, 400},
    {784, 200},
    {880, 400},
};

note_t inGameMelody3[] = {
    {659, 300},
    {784, 300},
    {880, 300},
    {987, 600},
    {880, 300},
    {987, 600},
};

note_t inGameMelody4[] = {
    {523, 200},
    {659, 200},
    {784, 200},
    {880, 400},
    {784, 200},
    {880, 400},
};

note_t inGameMelody5[] = {
    {523, 300},
    {659, 300},
    {784, 300},
    {880, 600},
};

note_t inGameMelody6[] = {
    {523, 400},
    {659, 400},
    {784, 400},
    {880, 800},
};

note_t victoryMelody1[] = {
    {784, 400},
    {987, 400},
    {1175, 400},
    {1397, 800},
};

note_t victoryMelody2[] = {
    {523, 300},
    {659, 300},
    {784, 300},
    {880, 600},
};

note_t victoryMelody3[] = {
    {587, 300},
    {698, 300},
    {784, 300},
    {880, 600},
};

note_t victoryMelody4[] = {
    {784, 100},
    {932, 100},
    {1046, 100},
    {1174, 200},
    {1397, 200},
    {1568, 100},
    {1865, 100},
    {2093, 200},
    {2349, 200},
    {3136, 400},
};

note_t victoryMelody5[] = {
    {659, 100},
    {784, 100},
    {988, 100},
    {1174, 200},
    {1318, 200},
    {1760, 100},
    {1976, 100},
    {2093, 200},
    {2637, 200},
    {3136, 400},
};

note_t victoryMelody6[] = {
	{523, 100},
	{622, 100},
	{784, 100},
	{880, 200},
	{1046, 200},
	{1174, 100},
	{1568, 100},
	{1760, 200},
	{2093, 200},
	{2637, 400},
};

note_t defeatMelody1[] = {
    {392, 200},
    {294, 200},
    {262, 200},
    {196, 400},
};

note_t defeatMelody2[] = {
    {523, 300},
    {392, 300},
    {294, 300},
    {233, 600},
};

note_t defeatMelody3[] = {
    {349, 300},
    {294, 300},
    {233, 300},
    {196, 600},
};

note_t melody[] = {
    {440, 500},
    {523, 500},
    {587, 500},
    {659, 500},
};

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

/*void PlayTone(uint16_t frequency, uint16_t duration) {
    __HAL_TIM_SET_AUTORELOAD(&htim3, 1000000 / frequency); // Calcula el período en función de la frecuencia
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 50); // Duty cycle del 50% (puedes ajustarlo según sea necesario)
    HAL_Delay(duration);
}*/

void PlayTone(uint16_t frequency, uint16_t duration) {
    SetTone(frequency); // Configura la frecuencia antes de la duración

    HAL_Delay(duration);

    // Detén la salida PWM al final de la duración del tono
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void SetTone(uint16_t frequency) {
    if (frequency == 0) {
        // Si la frecuencia es 0, deshabilita la salida PWM
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    } else {
        // Configura la frecuencia y el ciclo de trabajo para generar el tono
        __HAL_TIM_SET_AUTORELOAD(&htim3, 1000000 / frequency);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 50); // Duty cycle del 50% (puedes ajustarlo según sea necesario)
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // Inicia la salida PWM
    }
}

void PlayMelody(note_t *melody, int numNotes) {
    for (int i = 0; i < numNotes; i++) {
        PlayTone(melody[i].frequency, melody[i].duration);
    }
}

static void main_task(void *pvParameters){

	typedef enum {
		STATE_WELCOME_SHOW,
		STATE_WELCOME_DELAY,
		STATE_WELCOME_CLEAR,
		STATE_MENU_SHOW,
		STATE_MENU_HANDLE,
		STATE_MENU_CLEAR,
		STATE_GAME_0_SHOW,
		STATE_GAME_0_HANDLE,
		STATE_GAME_0_PLAY,
		STATE_GAME_1_SHOW,
		STATE_GAME_1_HANDLE,
		STATE_GAME_1_PLAY,
		STATE_GAME_2_SHOW,
		STATE_GAME_2_HANDLE,
		STATE_GAME_2_PLAY,
		STATE_GAME_3_SHOW,
		STATE_GAME_3_HANDLE,
		STATE_GAME_3_PLAY,
		TEST_DIE
	} main_state_t;

	static main_state_t main_state = STATE_GAME_0_PLAY;// STATE_WELCOME_SHOW;
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
				next_state = STATE_GAME_0_SHOW;
				break;
			case 2:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_1_SHOW;
				break;
			case 3:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_2_SHOW;
				break;
			case 4:
				main_state = STATE_MENU_CLEAR;
				next_state = STATE_GAME_3_SHOW;
				break;
			}
			break;
		case STATE_MENU_CLEAR:
			if(lcd_progressive_clear(FOUR_LINES)){
				main_state = next_state;
			}
			break;
		case STATE_GAME_0_SHOW:
			if(lcd_progressive_print("        PONG        ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_0_HANDLE;
			}
			break;
		case STATE_GAME_0_HANDLE:
			switch(menu_game_handle()){ // por ahora vuelvo en todos los casos...
			case 1: // jugar
				main_state = STATE_GAME_0_PLAY;
				break;
			case 2: // reglas
			case 3: // puntajes
			case 4: // volver
				main_state = TEST_DIE;
				break;
			}
			break;
		case STATE_GAME_0_PLAY:
			if(menu_game_play()){
				main_state = TEST_DIE;
			}
			break;
		case STATE_GAME_1_SHOW:
			if(lcd_progressive_print("       TETRIS       ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_1_HANDLE;
			}
			break;
		case STATE_GAME_1_HANDLE:
			switch(menu_game_handle()){ // por ahora vuelvo en todos los casos...
			case 1: // jugar
			case 2: // reglas
			case 3: // puntajes
			case 4: // volver
				main_state = TEST_DIE;
				break;
			}
			break;
		case STATE_GAME_2_SHOW:
			if(lcd_progressive_print("       SNAKE        ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_2_HANDLE;
			}
			break;
		case STATE_GAME_2_HANDLE:
			switch(menu_game_handle()){ // por ahora vuelvo en todos los casos...
			case 1: // jugar
			case 2: // reglas
			case 3: // puntajes
			case 4: // volver
				main_state = TEST_DIE;
				break;
			}
			break;
		case STATE_GAME_3_SHOW:
			if(lcd_progressive_print("       SPACE        ",
									 "Instrucciones del ju",
									 " Jugar      Reglas  ",
									 " Puntajes   Volver  ",
									 FOUR_LINES)){
				main_state = STATE_GAME_3_HANDLE;
			}
			break;
		case STATE_GAME_3_HANDLE:
			switch(menu_game_handle()){ // por ahora vuelvo en todos los casos...
			case 1: // jugar
			case 2: // reglas
			case 3: // puntajes
			case 4: // volver
				main_state = TEST_DIE;
				break;
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
/*
	DOT_MATRIX_Init(&hspi1);

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


	pong_init();
	pong_print_board();


	while(1){

		if(pong_play()){
			vTaskSuspend(NULL);
		}

	}*/
	while(1){
		// happyBirthdayMelody
		// jingleBellsMelody
		// twinkleTwinkleMelody
		// marioBrosMelody
		// harryPotterMelody
//		powerOnMelody1
//		powerOnMelody2
//		powerOnMelody3
//		inGameMelody1
//		inGameMelody2
//		inGameMelody3
//		victoryMelody1
//		victoryMelody2
//		victoryMelody3
//		defeatMelody1
//		defeatMelody2
//		defeatMelody3
		PlayMelody(victoryMelody6, (sizeof(victoryMelody6)/sizeof(note_t)));
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

static void game_task(void *pvParameters){

	uint8_t game_data;

	while(1){
		for(uint8_t i=0; i<10; i++){
			vTaskDelay(1000/portTICK_PERIOD_MS);
			game_data = PLAYER_1_POINT;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
			vTaskDelay(1000/portTICK_PERIOD_MS);
			game_data = PLAYER_2_POINT;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
		game_data = GAME_OVER;
		xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
		vTaskDelete(NULL);
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
/*
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

  xTaskCreate(game_task,
			  "game_task",
			  configMINIMAL_STACK_SIZE,
			  NULL,
			  1,
			  NULL);
*/

  xTaskCreate(test_task,
			  "test_task",
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
