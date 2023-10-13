/*
 * pong.c
 *
 *  Created on: 25 ago. 2023
 *      Author: hernan
 */

#include "pong.h"
#include "main.h"
#include "menu.h"
#include "joystick.h"
#include "DOT_MATRIX.h"
#include "FreeRTOS.h"
#include "queue.h"

player_t player_1;
player_t player_2;
ball_t ball;

extern QueueHandle_t joysticks_queue;
extern QueueHandle_t game_queue;

static uint32_t my_rand(void){
	uint32_t y = HAL_GetTick();
	y^= y<<13;
	y^= y>>17;
	y^= y<<5;
	return y;
}

static void regen_ball(void){
	ball.x = (my_rand()%7) + 12;
	ball.y = (my_rand()%20) + 5;
	ball.direction = my_rand()%4;
}

void pong_init(void){

	player_1.x = 0;
	player_1.y = (PONG_BOARD_HEIGHT-PADDLE_1_LENGTH)/2 + 1;
	player_1.score = 0;

	player_2.x = PONG_BOARD_WIDTH;
	player_2.y = (PONG_BOARD_HEIGHT-PADDLE_1_LENGTH)/2 + 1;
	player_2.score = 0;

	regen_ball();

}

void pong_play(void){
	typedef enum{
		PONG_FIRST_PRINT,
		PONG_COUNTDOWN,
		PONG_PLAYING,
		PONG_PLAYER_1_PAUSE,
		PONG_PLAYER_2_PAUSE,
		PONG_GAME_OVER
	} pong_state_t;

	uint8_t joystick;
	uint8_t game_data;
	static pong_state_t pong_state = PONG_FIRST_PRINT;

	switch(pong_state){
	case PONG_FIRST_PRINT:
		pong_print_board();
		pong_state = PONG_COUNTDOWN;
		break;
	case PONG_COUNTDOWN:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case SEC_3:
				MATRIX_print_num(MATRIX_DISPLAY_UNIT1, 3);
				break;
			case SEC_2:
				MATRIX_print_num(MATRIX_DISPLAY_UNIT1, 2);
				break;
			case SEC_1:
				MATRIX_print_num(MATRIX_DISPLAY_UNIT1, 1);
				break;
			case SEC_0:
				MATRIX_print_num(MATRIX_DISPLAY_UNIT1, 0);
				break;
			case GAME_START:
			case GAME_RESUME:
				pong_state = PONG_PLAYING;
				break;
			}
		}
		break;
	case PONG_PLAYING:
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)) {
			switch(joystick) {
				case JOYSTICK_1_UP:
					if(player_1.y > 0) {
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, player_1.y + PADDLE_1_LENGTH - 1, MATRIX_LED_OFF);
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, --player_1.y, MATRIX_LED_ON);
					}
					break;
				case JOYSTICK_1_DOWN:
					if(player_1.y + PADDLE_1_LENGTH - 1 < PONG_BOARD_HEIGHT) {
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, player_1.y, MATRIX_LED_OFF);
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, (++player_1.y) + PADDLE_1_LENGTH - 1, MATRIX_LED_ON);
					}
					break;
				case JOYSTICK_2_UP:
					if(player_2.y > 0) {
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, player_2.y + PADDLE_2_LENGTH - 1, MATRIX_LED_OFF);
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, --player_2.y, MATRIX_LED_ON);
					}
					break;
				case JOYSTICK_2_DOWN:
					if(player_2.y + PADDLE_2_LENGTH - 1 < PONG_BOARD_HEIGHT) {
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, player_2.y, MATRIX_LED_OFF);
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, (++player_2.y) + PADDLE_2_LENGTH - 1, MATRIX_LED_ON);
					}
					break;
				case JOYSTICK_1_PULS: // pausa
					game_data = PLAYER_1_PAUSE;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					pong_state = PONG_PLAYER_1_PAUSE;
					taskYIELD();
					break;
				case JOYSTICK_2_PULS: // pausa
					game_data = PLAYER_2_PAUSE;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					pong_state = PONG_PLAYER_2_PAUSE;
					taskYIELD();
					break;
			}
		}
		pong_move_ball(NO_TOUCH);
		switch(pong_change_ball_direction()){
		case 0: // no hay punto
			MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
			break;

		case 1: // punto player1
			MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
			ball_blink();
			pong_print_board();

			player_1.score++;
			game_data = PLAYER_1_POINT;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

			regen_ball();
			break;

		case 2: // punto player2
			MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
			ball_blink();
			pong_print_board();

			player_2.score++;
			game_data = PLAYER_2_POINT;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

			regen_ball();
			break;
		}

		if(player_1.score == END_SCORE){
			game_data = PLAYER_1_WIN;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
			pong_state = PONG_GAME_OVER;
		} else if(player_2.score == END_SCORE){
			game_data = PLAYER_2_WIN;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
			pong_state = PONG_GAME_OVER;
		}
		break;
	case PONG_PLAYER_1_PAUSE:
	case PONG_PLAYER_2_PAUSE:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case PLAYER_1_PAUSE:
			case PLAYER_2_PAUSE:
				game_data = PLAYER_1_PAUSE;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				taskYIELD();
				break;
			case GAME_COUNTDOWN:
				pong_state = PONG_COUNTDOWN;
				break;
			case GAME_RESUME:
				pong_state = PONG_PLAYING;
				break;
			case GAME_RESET:
				pong_state = PONG_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			case GAME_OVER:
				// TODO: hacer algun tipo de animacion con la pantalla de game over, mostrarla y morir.
				//MATRIX_print_game_over(MATRIX_DISPLAY_UNIT1);
				pong_state = PONG_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			}
		}
		break;
	case PONG_GAME_OVER:
		// TODO: mostrar alguna animacion
		//MATRIX_print_game_over(MATRIX_DISPLAY_UNIT1);
		pong_state = PONG_FIRST_PRINT;
		vTaskDelete(NULL); // chau chau adios...
		break;
	}
}

void ball_blink(void){
	for(uint8_t i=0; i<3; i++){
		vTaskDelay(100/portTICK_PERIOD_MS);
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, ball.x, ball.y, MATRIX_LED_OFF);
		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		vTaskDelay(100/portTICK_PERIOD_MS);
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, ball.x, ball.y, MATRIX_LED_ON);
		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
	}
}

void pong_print_board(void){
	MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
	for(uint8_t i=0; i<PADDLE_1_LENGTH; i++) {
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, player_1.y + i, MATRIX_LED_ON);
	}
	for(uint8_t i=0; i<PADDLE_2_LENGTH; i++) {
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, player_2.y + i, MATRIX_LED_ON);
	}
	MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
}

void pong_move_ball(uint8_t touch){
	if(touch){
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, ball.x, ball.y, MATRIX_LED_OFF);
	}
	switch(ball.direction) {
		case RIGHT_UP:
			ball.x++;
			ball.y--;
			break;
		case LEFT_UP:
			ball.x--;
			ball.y--;
			break;
		case RIGHT_DOWN:
			ball.x++;
			ball.y++;
			break;
		case LEFT_DOWN:
			ball.x--;
			ball.y++;
	}
	MATRIX_set_led(MATRIX_DISPLAY_UNIT1, ball.x, ball.y, MATRIX_LED_ON);
}

uint8_t pong_change_ball_direction(void){

	if(ball.y < 1 || ball.y > (PONG_BOARD_HEIGHT-1)) { // si toco borde inferior o superior
		ball.direction += ball.direction<2 ? 2 : -2;
	}
	if(ball.x <= player_1.x) { // si toco al player1
		if(player_1.y <= ball.y && ball.y < player_1.y + PADDLE_1_LENGTH){
			ball.direction += ball.direction%2 ? -1 : 1;
			pong_move_ball(TOUCH);
		} else { // si toco el borde de player1, punto para player2
			return 2;
		}
	} else if(ball.x >= player_2.x) { // si toco al player2
		if(player_2.y <= ball.y && ball.y < player_2.y + PADDLE_2_LENGTH){
			ball.direction += ball.direction%2 ? -1 : 1;
			pong_move_ball(TOUCH);
		} else { // si toco el borde de player2, punto para player1
			return 1;
		}
	}
	return 0;
}


