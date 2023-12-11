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
#include "buzzer.h"
#include "DOT_MATRIX.h"

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t joysticks_queue;
extern QueueHandle_t game_queue;
extern QueueHandle_t buzzer_queue;

const char rules_pong[6][20] = {
		"El juego simula un  ",
		"tenis de mesa. Cada ",
		"jugador controla su ",
		"paleta verticalmente",
		"El primero en llegar",
		"a 5 puntos, gana.   "
};

const char rules_snake[5][20] = {
		"Controla la vibora  ",
		"para comer las fruta",
		"Si chocas con el bor",
		"de perdes una vida. ",
		"Presiona para pausar"
};

const char rules_tetris[6][20] = {
		"Desplaza la pieza a ",
		"izquierda y derecha ",
		"mientras cae. Para  ",
		"rotarla, move el joy",
		"stick hacia arriba. ",
		"Presiona para pausar"
};

const char rules_conway[8][20] = {
		"Automata celular don",
		"de las celulas evolu",
		"cionan segun reglas ",
		"simples basadas en  ",
		"sus vecinas en una  ",
		"cuadricula, usado en",
		"matematicas y cs de ",
		"la computacion      "
};

uint8_t menu_game_handle(void){
	typedef enum{
		STATE_PLAY,
		STATE_RULES,
		STATE_SCORE,
		STATE_BACK
	}menu_game_state_t;

	uint8_t joystick, buzzer_data;
	static menu_game_state_t menu_game_state = STATE_PLAY;

	switch(menu_game_state){
	case STATE_PLAY:
		menu_blink(0, " Jugar    ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
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
			BUZZER_TONE();
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
		menu_blink(2, " Puntajes ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(2, " Puntajes ");
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
			BUZZER_TONE();
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
	uint8_t joystick, buzzer_data;

	switch(menu_state){
	case STATE_GAME_0:
		menu_blink(0, "  Pong    ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
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
			BUZZER_TONE();
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
			BUZZER_TONE();
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
		menu_blink(3, "  Conway  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(3, "  Conway  ");
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

uint8_t menu_game_play(uint8_t game, const char* text){
	typedef enum{
		STATE_COUNTDOWN_SHOW,
		STATE_COUNTDOWN_RESUME_SHOW,
		STATE_COUNTDOWN_HANDLE,
		STATE_PLAYING_HANDLE,
		STATE_PAUSE_SHOW,
		STATE_PAUSE_HANDLE,
		STATE_GAMEOVER_SHOW,
		STATE_GAMEOVER_WIN_SHOW,
		STATE_GAMEOVER_LOST_SHOW,
		STATE_GAMEOVER_HANDLE
	}game_play_t;

	static uint32_t seconds=0, minutes=0;
	static uint32_t score_1=0, score_2=0, score=0, lives=3;
	static uint32_t start_ticks=0;
	static int32_t countdown=3;
	static game_play_t game_play = STATE_COUNTDOWN_SHOW;
	uint8_t game_data, buzzer_data;

	switch(game_play){
	case STATE_COUNTDOWN_SHOW:
		if(lcd_progressive_print(text,
								 "El juego comienza en",
								 "         3          ",
								 "                    ",
								 FOUR_LINES)){
			switch(game){
			case 0:
				xTaskCreate(pong_task,
							"pong_task",
							configMINIMAL_STACK_SIZE,
							NULL,
							1,
							NULL);
				break;
			case 1:
				xTaskCreate(tetris_task,
							"tetris_task",
							configMINIMAL_STACK_SIZE,
							NULL,
							1,
							NULL);
				break;
			case 2:
				xTaskCreate(snake_task,
							"snake_task",
							configMINIMAL_STACK_SIZE,
							NULL,
							1,
							NULL);
				break;
			case 3:
				xTaskCreate(conway_task,
							"conway_task",
							configMINIMAL_STACK_SIZE,
							NULL,
							1,
							NULL);
				break;
			}
			game_play = STATE_COUNTDOWN_HANDLE;
			countdown=3;
			game_data = SEC_3;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
			start_ticks = HAL_GetTick();
		}
		break;
	case STATE_COUNTDOWN_RESUME_SHOW:
		if(lcd_progressive_print(text,
								 "El juego continua en",
								 "         3          ",
								 "                    ",
								 FOUR_LINES)){
			game_play = STATE_COUNTDOWN_HANDLE;
			countdown=3;

			game_data = SEC_3;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

			BUZZER_TONE();
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

				game_data = SEC_2;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

				BUZZER_TONE();
			} else if(countdown==1){
				lcd_set_cursor(2, 9);
				lcd_char('1');

				game_data = SEC_1;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

				BUZZER_TONE();
			} else if(countdown==0){
				lcd_set_cursor(2, 9);
				lcd_char('0');

				game_data = SEC_0;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

				BUZZER_TONE();
			} else if(countdown<0){
				BUZZER_LONG_TONE();
				switch(game){
				case 0:
					lcd_print(text,
							  "  Partida en juego  ",
							  "   P1:00     P2:00  ",
							  "        00:00       ");
					lcd_print_score(score_1, score_2);
					break;
				case 1:
					lcd_print(text,
							  "  Partida en juego  ",
							  "    Puntos:0000     ",
							  "        00:00       ");
					lcd_print_pts(score);
					break;
				case 2:
					lcd_print(text,
							  "  Partida en juego  ",
							  "   Pts:00  Vidas:3  ",
							  "        00:00       ");
					lcd_print_pts_lives(score, lives);
					break;
				case 3:
					lcd_print(text,
							  " El juego de la vida",
							  "                    ",
							  "        00:00       ");
					break;
				}
				if(seconds == 0 && minutes == 0){ // juego nuevo
					game_data = GAME_START;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					game_play = STATE_PLAYING_HANDLE;
					taskYIELD();
				} else { // juego continua
					game_data = GAME_RESUME;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					game_play = STATE_PLAYING_HANDLE;
					taskYIELD();
				}
			}
		}
		break;
	case STATE_PLAYING_HANDLE:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case TETRIS_LINE:
				score+=100;
				break;
			case SINGLE_PLAYER_POINT:
				if(game == 1){
					score+=10;
				} else {
					score++;
				}
				break;
			case SINGLE_PLAYER_LIVE:
				lives--;
				break;
			case PLAYER_1_POINT:
				score_1++;
				break;
			case PLAYER_2_POINT:
				score_2++;
				break;
			case PLAYER_1_PAUSE:
			case PLAYER_2_PAUSE:
			case SINGLE_PLAYER_PAUSE:
				game_play = STATE_PAUSE_SHOW;
				break;
			case PLAYER_1_WIN:
			case PLAYER_2_WIN:
				game_play = STATE_GAMEOVER_SHOW;
				break;
			case SINGLE_PLAYER_WIN:
				game_play = STATE_GAMEOVER_WIN_SHOW;
				break;
			case SINGLE_PLAYER_LOST:
				game_play = STATE_GAMEOVER_LOST_SHOW;
				break;
			case GAME_OVER:
				return 1;
				break;
			}
			switch(game){
			case 0:
				lcd_print_score(score_1, score_2);
				break;
			case 1:
				lcd_print_pts(score);
				break;
			case 2:
				lcd_print_pts_lives(score, lives);
				break;
			}
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
		if(lcd_progressive_print(text,
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
				game_data = GAME_COUNTDOWN;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				game_play = STATE_COUNTDOWN_RESUME_SHOW;
				break;
			case 2:
				RESET_GAME_STATE();
				game_data = GAME_RESET;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				game_play = STATE_COUNTDOWN_SHOW;
				break;
			case 3: // puntajes TODO: no se si vale la pena, creo que es un bardo
			case 4: // salir
				game_data = GAME_OVER;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				if(game==0){
					game_play = STATE_GAMEOVER_SHOW;
				} else {
					game_play = STATE_GAMEOVER_LOST_SHOW;
				}

				break;
			}
		break;
		case STATE_GAMEOVER_SHOW:
			lcd_print(text,
					  " Partida finalizada ",
					  "   P1:00     P2:00  ",
					  " Reiniciar   Salir  ");
			lcd_print_score(score_1, score_2);
			game_play = STATE_GAMEOVER_HANDLE;
			break;
		case STATE_GAMEOVER_WIN_SHOW:
			lcd_print(text,
					  "  Felicitaciones!   ",
					  " Ganaste la partida!",
					  " Reiniciar   Salir  ");
			game_play = STATE_GAMEOVER_HANDLE;
			break;
		case STATE_GAMEOVER_LOST_SHOW:
			lcd_print(text,
					  "   Buen intento!    ",
					  " Segui participando ",
					  " Reiniciar   Salir  ");
			game_play = STATE_GAMEOVER_HANDLE;
			break;
		case STATE_GAMEOVER_HANDLE:
			switch(menu_gameover_handle()){
			case 1: // volver a jugar
				RESET_GAME_STATE();
				game_data = GAME_RESET;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

				MATRIX_CLEAR(MATRIX_DISPLAY_UNIT1);
				game_play = STATE_COUNTDOWN_SHOW;

				if(game == 3) {
					MATRIX_print_msg(MATRIX_DISPLAY_UNIT1, CONWAY_MSG);
					MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
					return 2; // reiniciar hace que vuelvas a elegir el patron
				}
				break;
			case 2: // salir
				RESET_GAME_STATE();

			    MATRIX_print_msg(MATRIX_DISPLAY_UNIT1, WELCOME_MSG);
			    MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);

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
	uint8_t joystick, buzzer_data;

	switch(gameover_state){
	case STATE_RESET:
		menu_blink(2, " Reiniciar");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
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
			BUZZER_TONE();
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
	uint8_t joystick, buzzer_data;

	switch(pause_state){
	case STATE_CONTINUE:
		menu_blink(0, " Reanudar ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
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
			BUZZER_TONE();
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
			BUZZER_TONE();
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
			BUZZER_TONE();
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

uint8_t menu_conway_options_handle(void){
	typedef enum{
		CONWAY_4_8_12,
		CONWAY_4_GLIDER,
		CONWAY_JASON_P22,
		CONWAY_RANDOM
	}conway_menu_state_t;

	uint8_t joystick, buzzer_data;
	static conway_menu_state_t conway_menu_state = CONWAY_4_8_12;

	switch(conway_menu_state){
	case CONWAY_4_8_12:
		menu_blink(0, "  4-8-12  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(0, "  4-8-12  ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				conway_menu_state = CONWAY_4_8_12;
				return CONWAY_4_8_12+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				conway_menu_state = CONWAY_4_GLIDER;
			} else if(joystick == JOYSTICK_1_DOWN){
				conway_menu_state = CONWAY_JASON_P22;
			}
		}
		break;
	case CONWAY_4_GLIDER:
		menu_blink(1, " 4 Glider ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(1, " 4 Glider ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				conway_menu_state = CONWAY_4_8_12;
				return CONWAY_4_GLIDER+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				conway_menu_state = CONWAY_4_8_12;
			} else if(joystick == JOYSTICK_1_DOWN){
				conway_menu_state = CONWAY_RANDOM;
			}
		}
		break;
	case CONWAY_JASON_P22:
		menu_blink(2, " Jason P22");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(2, " Jason P22");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				conway_menu_state = CONWAY_4_8_12;
				return CONWAY_JASON_P22+1;
			} else if(joystick == JOYSTICK_1_RIGHT){
				conway_menu_state = CONWAY_RANDOM;
			} else if(joystick == JOYSTICK_1_UP){
				conway_menu_state = CONWAY_4_8_12;
			}
		}
		break;
	case CONWAY_RANDOM:
		menu_blink(3, "   Random ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(3, "   Random ");
			if(joystick == JOYSTICK_1_PULS){
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				conway_menu_state = CONWAY_4_8_12;
				return CONWAY_RANDOM+1;
			} else if(joystick == JOYSTICK_1_LEFT){
				conway_menu_state = CONWAY_JASON_P22;
			} else if(joystick == JOYSTICK_1_UP){
				conway_menu_state = CONWAY_4_GLIDER;
			}
		}
		break;
	}
	return 0;
}

uint8_t menu_rules_handle (uint8_t game) {
	typedef enum{
		STATE_RULES_PLAY,
		STATE_RULES_BACK
	}rules_state_t;

	static rules_state_t rules_state = STATE_RULES_PLAY;
	static uint8_t index_rules = 2;
	uint8_t joystick, buzzer_data, limit;
	const char (*lines)[20];

	switch(game){
	case 0:
		lines = rules_pong;
		limit = LINES_RULES_PONG;
		break;
	case 1:
		lines = rules_tetris;
		limit = LINES_RULES_TETRIS;
		break;
	case 2:
		lines = rules_snake;
		limit = LINES_RULES_SNAKE;
		break;
	case 3:
		lines = rules_conway;
		limit = LINES_RULES_CONWAY;
		break;
	}

	switch(rules_state){
	case STATE_RULES_PLAY:
		menu_blink(2, "   Jugar  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(2, "   Jugar  ");
			switch(joystick){
			case JOYSTICK_1_PULS:
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				rules_state = STATE_RULES_PLAY;
				return STATE_RULES_PLAY+1;
				break;
			case JOYSTICK_1_RIGHT:
				rules_state = STATE_RULES_BACK;
				break;
			case JOYSTICK_1_DOWN:
				if(index_rules < limit-1){
					index_rules++;
					lcd_print(lines[index_rules-2],
							  lines[index_rules-1],
							  lines[index_rules],
							  "   Jugar    Volver  ");
				}

				break;
			case JOYSTICK_1_UP:
				if(index_rules > 2){
					index_rules--;
					lcd_print(lines[index_rules-2],
							  lines[index_rules-1],
							  lines[index_rules],
							  "   Jugar    Volver  ");
				}

				break;
			}
		}
		break;
	case STATE_RULES_BACK:
		menu_blink(3, "  Volver  ");
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			BUZZER_TONE();
			menu_blink_option(3, "  Volver  ");
			switch(joystick){
			case JOYSTICK_1_PULS:
				xQueueReceive(joysticks_queue, &joystick, 0); // por si hay un espurio
				rules_state = STATE_RULES_PLAY;
				return STATE_RULES_BACK+1;
				break;
			case JOYSTICK_1_LEFT:
				rules_state = STATE_RULES_PLAY;
				break;
			case JOYSTICK_1_DOWN:
				if(index_rules < limit){
					index_rules++;
					lcd_print(lines[index_rules],
							  lines[index_rules+1],
							  lines[index_rules+2],
							  "   Jugar    Volver  ");
				}
				break;
			case JOYSTICK_1_UP:
				if(index_rules > 2){
					index_rules--;
					lcd_print(lines[index_rules],
							  lines[index_rules+1],
							  lines[index_rules+2],
							  "   Jugar    Volver  ");
				}
				break;
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
