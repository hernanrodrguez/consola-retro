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
		menu_blink(0, " Jugar    ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(0, " Jugar    ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
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

uint8_t menu_game_0_play(void){
	typedef enum{
		STATE_COUNTDOWN_SHOW,
		STATE_COUNTDOWN_RESUME_SHOW,
		STATE_COUNTDOWN_HANDLE,
		STATE_PLAYING_HANDLE,
		STATE_PAUSE_SHOW,
		STATE_PAUSE_HANDLE,
		STATE_GAMEOVER_SHOW,
		STATE_GAMEOVER_HANDLE
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
			countdown=3;
			start_ticks = HAL_GetTick();
		}
		break;
	case STATE_COUNTDOWN_RESUME_SHOW:
		if(lcd_progressive_print("        PONG        ",
								 "El juego continua en",
								 "         3          ",
								 "                    ",
								 FOUR_LINES)){
			game_play = STATE_COUNTDOWN_HANDLE;
			countdown=3;
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
				lcd_print_score(score_1, score_2);
				if(seconds == 0 && minutes == 0){ // juego nuevo
					if( pdPASS == xTaskCreate(game_task,
											  "game_task",
											  configMINIMAL_STACK_SIZE,
											  NULL,
											  1,
											  NULL) ){
						game_play = STATE_PLAYING_HANDLE;
					}
				} else {
					game_play = STATE_PLAYING_HANDLE;
				}
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
			case PLAYER_1_PAUSE:
			case PLAYER_2_PAUSE:
				game_play = STATE_PAUSE_SHOW;
				break;
			case PLAYER_1_WIN:
			case PLAYER_2_WIN:
				game_play = STATE_GAMEOVER_SHOW;
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
	case STATE_PAUSE_SHOW:
		if(lcd_progressive_print("        PONG        ",
								 "  Partida en pausa  ",
								 " Reanudar  Reiniciar",
								 " Puntajes    Salir  ",
								 FOUR_LINES)){
			game_play = STATE_PAUSE_HANDLE;
		}
		break;
	case STATE_PAUSE_HANDLE:
		switch(menu_pause_handle()){
			case 1:
				game_data = GAME_RESUME;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

				game_play = STATE_COUNTDOWN_RESUME_SHOW;
				break;
			case 2:
				seconds=0;
				minutes=0;
				score_1=0;
				score_2=0;
				game_data = GAME_RESET;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				game_play = STATE_COUNTDOWN_SHOW;
				break;
			case 3: // puntajes TODO: no se si vale la pena, creo que es un bardo
			case 4: // salir
				game_data = GAME_OVER;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				game_play = STATE_GAMEOVER_SHOW;
				break;
			}
		break;
		case STATE_GAMEOVER_SHOW:
			lcd_print("        PONG        ",
					  " Partida finalizada ",
					  "   P1:00     P2:00  ",
					  " Reiniciar   Salir  ");
			lcd_print_score(score_1, score_2); // TODO: resolver mejor esto
			game_play = STATE_GAMEOVER_HANDLE;
			break;
		case STATE_GAMEOVER_HANDLE:
			switch(menu_gameover_handle()){
			case 1: // volver a jugar
				// reinicio los puntajes y eso
				seconds=0;
				minutes=0;
				score_1=0;
				score_2=0;

				game_play = STATE_COUNTDOWN_SHOW;
				break;
			case 2: // salir
				seconds=0;
				minutes=0;
				score_1=0;
				score_2=0;

				game_play = STATE_COUNTDOWN_SHOW;
				return 1;
				break;
			}
			break;
	}
	return 0;
}

uint8_t menu_gameover_handle(void){
	typedef enum{
		STATE_RESET,
		STATE_EXIT
	}gameover_state_t;

	static gameover_state_t gameover_state = STATE_RESET;
	uint8_t joystick;

	switch(gameover_state){
	case STATE_RESET:
		menu_blink(2, " Reiniciar");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(2, " Reiniciar");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				gameover_state = STATE_RESET;
				return STATE_RESET+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				gameover_state = STATE_EXIT;
			}
		}
		break;
	case STATE_EXIT:
		menu_blink(3, "   Salir  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(3, "   Salir  ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				gameover_state = STATE_RESET;
				return STATE_EXIT+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				gameover_state = STATE_RESET;
			}
		}
		break;
	}
	return 0;
}

uint8_t menu_pause_handle(void){
	typedef enum{
		STATE_CONTINUE,
		STATE_RESET,
		STATE_SCORES,
		STATE_EXIT
	}pause_state_t;

	static pause_state_t pause_state = STATE_CONTINUE;
	uint8_t joystick;

	switch(pause_state){
	case STATE_CONTINUE:
		menu_blink(0, " Reanudar ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(0, " Reanudar ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				pause_state = STATE_CONTINUE;
				return STATE_CONTINUE+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				pause_state = STATE_RESET;
			} else if(joystick == JOYSTICK_1_DOWN){
				pause_state = STATE_SCORES;
			}
		}
		break;
	case STATE_RESET:
		menu_blink(1, " Reiniciar");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(1, " Reiniciar");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				pause_state = STATE_CONTINUE;
				return STATE_RESET+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				pause_state = STATE_CONTINUE;
			} else if(joystick == JOYSTICK_1_DOWN){
				pause_state = STATE_EXIT;
			}
		}
		break;
	case STATE_SCORES:
		menu_blink(2, " Puntajes ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(2, " Puntajes ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				pause_state = STATE_CONTINUE;
				return STATE_SCORES+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				pause_state = STATE_EXIT;
			} else if(joystick == JOYSTICK_1_UP){
				pause_state = STATE_CONTINUE;
			}
		}
		break;
	case STATE_EXIT:
		menu_blink(3, "   Salir  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			menu_blink_option(3, "   Salir  ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				pause_state = STATE_CONTINUE;
				return STATE_EXIT+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				pause_state = STATE_SCORES;
			} else if(joystick == JOYSTICK_1_UP){
				pause_state = STATE_RESET;
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
