/*
 * menu.c
 *
 *  Created on: 19 ago. 2023
 *      Author: hernan
 */

#include "menu.h"
#include "lcd1602_i2c.h"
#include "main.h"
#include "joystick.h"

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t joysticks_queue;

uint8_t menu_handle(void){
	typedef enum{
		STATE_GAME_0,
		STATE_GAME_1,
		STATE_GAME_2,
		STATE_GAME_3
	}menu_state_t;

	static menu_state_t menu_state = STATE_GAME_0;
	uint8_t joystick;

	switch(menu_state){
	case STATE_GAME_0:
		menu_blink(0, "  Pong    ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(0, "  Pong    ");
			if(joystick == JOYSTICK_1_PULS){
				menu_state = STATE_GAME_0;
				return STATE_GAME_0+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				menu_state = STATE_GAME_1;
			} else if(joystick == JOYSTICK_1_DOWN){
				menu_state = STATE_GAME_2;
			}
		}
		break;
	case STATE_GAME_1:
		menu_blink(1, "  Tetris  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(1, "  Tetris  ");
			if(joystick == JOYSTICK_1_PULS){
				menu_state = STATE_GAME_0;
				return STATE_GAME_1+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				menu_state = STATE_GAME_0;
			} else if(joystick == JOYSTICK_1_DOWN){
				menu_state = STATE_GAME_3;
			}
		}
		break;
	case STATE_GAME_2:
		menu_blink(2, "  Snake   ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(2, "  Snake   ");
			if(joystick == JOYSTICK_1_PULS){
				menu_state = STATE_GAME_0;
				return STATE_GAME_2+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				menu_state = STATE_GAME_3;
			} else if(joystick == JOYSTICK_1_UP){
				menu_state = STATE_GAME_0;
			}
		}
		break;
	case STATE_GAME_3:
		menu_blink(3, "  Space   ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(3, "  Space   ");
			if(joystick == JOYSTICK_1_PULS){
				menu_state = STATE_GAME_0;
				return STATE_GAME_3+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				menu_state = STATE_GAME_2;
			} else if(joystick == JOYSTICK_1_UP){
				menu_state = STATE_GAME_1;
			}
		}
		break;
	}

	return 0;
}

void menu_blink_option(uint8_t option, const char *text){
	uint8_t i, j;

	if(option == 0){
		i=2;
		j=0;
	} else if(option == 1){
		i=2;
		j=10;
	} else if(option == 2){
		i=3;
		j=0;
	} else if(option == 3){
		i=3;
		j=10;
	}

	lcd_set_cursor(i, j);
	lcd_string(text);
}

void menu_blink(uint8_t option, const char *text){
	typedef enum {
		BLINK_ON,
		BLINK_OFF
	}blink_state_t;

	static blink_state_t blink_state = BLINK_OFF;
	static uint32_t start_ticks = 0;

	switch(blink_state){
	case BLINK_ON:
		if(HAL_GetTick() - start_ticks > BLINK_RATE){
			menu_blink_option(option, text);
			start_ticks = HAL_GetTick();
			blink_state = BLINK_OFF;
		}
		break;
	case BLINK_OFF:
		if(HAL_GetTick() - start_ticks > BLINK_RATE){
			menu_blink_option(option, "          ");
			start_ticks = HAL_GetTick();
			blink_state = BLINK_ON;
		}
		break;
	}
}
