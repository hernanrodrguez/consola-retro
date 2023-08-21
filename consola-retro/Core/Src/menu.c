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
extern QueueHandle_t game_queue;

uint8_t menu_game_handle(void){
	typedef enum{
		STATE_PLAY,
		STATE_RULES,
		STATE_SCORE,
		STATE_BACK
	}menu_game_state_t;

	uint8_t joystick;
	static menu_game_state_t menu_game_state = STATE_PLAY;

	switch(menu_game_state){
	case STATE_PLAY:
		menu_blink(0, "  Jugar   ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(0, "  Jugar   ");
			if(joystick == JOYSTICK_1_PULS){
				menu_game_state = STATE_PLAY;
				return STATE_PLAY+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				menu_game_state = STATE_RULES;
			} else if(joystick == JOYSTICK_1_DOWN){
				menu_game_state = STATE_SCORE;
			}
		}
		break;
	case STATE_RULES:
		menu_blink(1, "  Reglas  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(1, "  Reglas  ");
			if(joystick == JOYSTICK_1_PULS){
				menu_game_state = STATE_PLAY;
				return STATE_RULES+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				menu_game_state = STATE_PLAY;
			} else if(joystick == JOYSTICK_1_DOWN){
				menu_game_state = STATE_BACK;
			}
		}
		break;
	case STATE_SCORE:
		menu_blink(2, "  Puntajes");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(2, "  Puntajes");
			if(joystick == JOYSTICK_1_PULS){
				menu_game_state = STATE_PLAY;
				return STATE_SCORE+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				menu_game_state = STATE_BACK;
			} else if(joystick == JOYSTICK_1_UP){
				menu_game_state = STATE_PLAY;
			}
		}
		break;
	case STATE_BACK:
		menu_blink(3, "  Volver  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(3, "  Volver  ");
			if(joystick == JOYSTICK_1_PULS){
				menu_game_state = STATE_PLAY;
				return STATE_BACK+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				menu_game_state = STATE_SCORE;
			} else if(joystick == JOYSTICK_1_UP){
				menu_game_state = STATE_RULES;
			}
		}
		break;
	}
	return 0;
}

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

uint8_t menu_game_play(void){
	typedef enum{
		STATE_COUNTDOWN_SHOW,
		STATE_COUNTDOWN_HANDLE,
		STATE_PLAYING_HANDLE,
		STATE_GAMEOVER
	}game_play_t;

	static uint32_t seconds=0, minutes=0;
	static uint32_t score_1=0, score_2=0;
	static uint32_t start_ticks=0;
	static int32_t countdown=3;
	static game_play_t game_play = STATE_COUNTDOWN_SHOW;
	uint8_t game_data;

	switch(game_play){
	case STATE_COUNTDOWN_SHOW:
		if(lcd_progressive_print("        PONG        ",
								 "El juego comienza en",
								 "         3          ",
								 "                    ",
								 FOUR_LINES)){
			game_play = STATE_COUNTDOWN_HANDLE;
			start_ticks = HAL_GetTick();
		}
		break;
	case STATE_COUNTDOWN_HANDLE:
		if(HAL_GetTick() - start_ticks > ONE_SECOND){
			countdown--;
			start_ticks = HAL_GetTick();
			if(countdown==2){
				lcd_set_cursor(2, 9);
				lcd_char('2');
			} else if(countdown==1){
				lcd_set_cursor(2, 9);
				lcd_char('1');
			} else if(countdown==0){
				lcd_set_cursor(2, 9);
				lcd_char('0');
			} else if(countdown<0){
				lcd_print("        PONG        ",
						  "  Partida en juego  ",
						  "   P1:00     P2:00  ",
						  "        00:00       ");
				game_play = STATE_PLAYING_HANDLE;
			}
		}
		break;
	case STATE_PLAYING_HANDLE:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case PLAYER_1_POINT:
				score_1++;
				break;
			case PLAYER_2_POINT:
				score_2++;
				break;
			case GAME_OVER:
				return 1;
				break;
			}
			lcd_print_score(score_1, score_2);
		}
		if(HAL_GetTick() - start_ticks > ONE_SECOND){
			seconds++;
			if(seconds>59){
				seconds = 0;
				minutes++;
			}
			lcd_print_time(minutes, seconds);
			start_ticks = HAL_GetTick();
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
