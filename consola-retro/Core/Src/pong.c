/*
 * pong.c
 *
 *  Created on: 25 ago. 2023
 *      Author: hernan
 */

#include "pong.h"
#include "main.h"
#include "joystick.h"
#include "DOT_MATRIX.h"
#include "FreeRTOS.h"
#include "queue.h"

player_t player_1;
player_t player_2;
ball_t ball;

extern QueueHandle_t joysticks_queue;

static uint32_t my_rand(void){
	uint32_t y = HAL_GetTick();
	y^= y<<13;
	y^= y>>17;
	y^= y<<5;
	return y;
}

void pong_init(void){

	player_1.x = PONG_BOARD_WIDTH;
	player_1.y = (PONG_BOARD_HEIGHT-PADDLE_1_LENGTH)/2 + 6;
	player_1.score = 0;

	player_2.x = 0;
	player_2.y = (PONG_BOARD_HEIGHT-PADDLE_1_LENGTH)/2 + 6;
	player_2.score = 0;

	ball.x = PONG_BOARD_WIDTH/2 + 10;
	ball.y = PONG_BOARD_HEIGHT/2 + 5;
	ball.direction = RIGHT_DOWN;

}

//void pong_opening_screen(void);

uint8_t pong_play(void){
	uint8_t joystick;

	if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)) {
		switch(joystick) {
			case JOYSTICK_1_UP:
				if(player_1.y > 6) {
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, player_1.y + PADDLE_1_LENGTH - 1, MATRIX_LED_OFF);
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, --player_1.y, MATRIX_LED_ON);
				}
				break;
			case JOYSTICK_1_DOWN:
				if(player_1.y + PADDLE_1_LENGTH < 5 + PONG_BOARD_HEIGHT) {
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, player_1.y, MATRIX_LED_OFF);
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, (++player_1.y) + PADDLE_1_LENGTH - 1, MATRIX_LED_ON);
				}
				break;
			case JOYSTICK_2_UP:
				if(player_2.y > 6) {
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, player_2.y + PADDLE_2_LENGTH - 1, MATRIX_LED_OFF);
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, --player_2.y, MATRIX_LED_ON);
				}
				break;
			case JOYSTICK_2_DOWN:
				if(player_2.y + PADDLE_2_LENGTH < 5 + PONG_BOARD_HEIGHT) {
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, player_2.y, MATRIX_LED_OFF);
					MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, (++player_2.y) + PADDLE_2_LENGTH - 1, MATRIX_LED_ON);
				}
				break;
			/*case JOYSTICK_1_PULS:
			case JOYSTICK_2_PULS: // pausa
				do {
					key = getch();
					if(key == ESCAPE_KEY) {
						MoveCursorToXY(0, 5 + PONG_BOARD_HEIGHT);
						exit(0);
					}
				} while(key != PAUSE_KEY);
				break;
			case ESCAPE_KEY:	// exit the game
				MoveCursorToXY(0, 5 + PONG_BOARD_HEIGHT);
				exit(0);*/
		}
	}
	pong_move_ball();
	pong_change_ball_direction();
	MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);

	if(player_1.score == END_SCORE || player_2.score == END_SCORE){
		return 1;
	} else {
		return 0;
	}

	/*system("cls");
	printf( "Game over Player %d!"				// game over
			"\nBetter luck next time."
			"\n(Press C to continue...)", (player1.score == END_SCORE) + 1);
	char key;
	do {
		key = getch();
		if(key == ESCAPE_KEY)
			exit(0);
	} while(key!='c' && key!='C');
	system("cls");*/
}

void pong_print_board(void){
	for(uint8_t i=0; i<PADDLE_1_LENGTH; i++) {
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_1.x, player_1.y + i, MATRIX_LED_ON);
	}
	for(uint8_t i=0; i<PADDLE_2_LENGTH; i++) {
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, player_2.x, player_2.y + i, MATRIX_LED_ON);
	}
	MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
	MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
}

void pong_move_ball(void){
	MATRIX_set_led(MATRIX_DISPLAY_UNIT1, ball.x, ball.y, MATRIX_LED_OFF);
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

void pong_change_ball_direction(void){
	if(ball.y < 1 || ball.y > (PONG_BOARD_HEIGHT-1)) {
		ball.direction += ball.direction<2 ? 2 : -2;
	}
	if(ball.x <= player_1.x + 1) {
		if(player_1.y <= ball.y && ball.y < player_1.y + PADDLE_1_LENGTH){
			ball.direction += ball.direction%2 ? -1 : 1;
		} else {
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, ball.x, ball.y, MATRIX_LED_ON); // PUNTO!
			vTaskDelay(400/portTICK_PERIOD_MS); // hacer que parpadee la pelota y se muestre algun mensaje
			ball.x = PONG_BOARD_WIDTH/2 + 10;
			ball.y = my_rand()%(PONG_BOARD_HEIGHT - 3) + 7;
			ball.direction = my_rand()%4;
			player_2.score++;
			/*
			if(++player_2.score == END_SCORE) {
				game_on = 0; // termino el partido
			}
			*/
		}
	} else if(ball.x >= player_2.x - 1) {
		if(player_2.y <= ball.y && ball.y < player_2.y + PADDLE_2_LENGTH){
			ball.direction += ball.direction%2 ? -1 : 1;
		} else {
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, ball.x, ball.y, MATRIX_LED_ON); // PUNTO!
			vTaskDelay(400/portTICK_PERIOD_MS); // hacer que parpadee la pelota y se muestre algun mensaje
			ball.x = PONG_BOARD_WIDTH/2 + 10;
			ball.y = my_rand()%(PONG_BOARD_HEIGHT - 3) + 7;
			ball.direction = my_rand()%4;
			player_1.score++;
			/*
			if(++player_1.score == END_SCORE){
				game_on = 0;
			}
			*/
		}
	}
}


