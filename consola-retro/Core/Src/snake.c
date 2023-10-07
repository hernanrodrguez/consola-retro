/*
 * snake.c
 *
 *  Created on: 18 sep. 2023
 *      Author: hernan
 */

#include "snake.h"
#include "main.h"
#include "joystick.h"
#include "DOT_MATRIX.h"
#include "FreeRTOS.h"
#include "queue.h"
/*
const point_t SPAWN_POINT = (point_t){25, 15, JOYSTICK_1_RIGHT};

// TODO: una funcion que me ponga los valores por defecto de snake y food
point_t head, bend[500], food, body[MAX_BODY_LENGTH+1];
snake_t snake = (snake_t){0, 0, DEFAULT_BODY_LENGTH + 1, SNAKE_DEFAULT_LIVES, 0};
uint8_t game_on_flag_2 = 1; // TODO: revisar como lo habia resuelto con el pong

extern QueueHandle_t joysticks_queue;
extern QueueHandle_t game_queue;

static uint32_t my_rand(void){
	uint32_t y = HAL_GetTick();
	y^= y<<13;
	y^= y>>17;
	y^= y<<5;
	return y;
}

uint8_t snake_play(void) {
	typedef enum{
		SNAKE_PLAYING,
		SNAKE_PAUSE,
		SNAKE_GAME_OVER
	} snake_state_t;

	uint8_t joystick;
	static snake_state_t snake_state = SNAKE_PLAYING;

	switch(snake_state){
	case SNAKE_PLAYING:
		snake_generate_food();
		snake.body_no = TEST_DEF;
		// Sleep(35);				// FPS: 1000 ms / 35 ms = 28.571428
		// TODO: hace falta hacerle un sleep?
		snake_move();
		lose_a_life();
		if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
			switch(joystick) {
				case JOYSTICK_1_LEFT:
				case JOYSTICK_1_RIGHT:
					switch(head.direction) {
						case JOYSTICK_1_UP:
						case JOYSTICK_1_DOWN:
							bend[++snake.bend_no] = head;
							head.direction = joystick; // TODO: revisar esto, seguro esta mal
							switch(joystick) {
								case JOYSTICK_1_LEFT:
									head.x--;
									break;
								case JOYSTICK_1_RIGHT:
									head.x++;
									break;
								case JOYSTICK_1_UP:
									head.y--;
									break;
								case JOYSTICK_1_DOWN:
									head.y++;
							}
					}
					break;
				case JOYSTICK_1_UP:
				case JOYSTICK_1_DOWN:
					switch(head.direction) {
						case JOYSTICK_1_LEFT:
						case JOYSTICK_1_RIGHT:
							bend[++snake.bend_no] = head;
							head.direction = joystick; // TODO: corregir, seguro esta mal
							switch(joystick) {
								case JOYSTICK_1_LEFT:
									head.x--;
									break;
								case JOYSTICK_1_RIGHT:
									head.x++;
									break;
								case JOYSTICK_1_UP:
									head.y--;
									break;
								case JOYSTICK_1_DOWN:
									head.y++;
							}
					}
					break;
				case JOYSTICK_1_PULS: // pausa
					game_data = PLAYER_1_PAUSE;
					xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
					snake_state = SNAKE_PAUSE;
					break;
			}
		}
		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		if(game_on_flag_2){
			return 0;
		} else {
			return 1;
		}
		break;
	case SNAKE_PAUSE:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case GAME_RESUME:
				snake_state = SNAKE_PLAYING; // TODO: countdown
				break;
			case GAME_RESET:
				// TODO: un init para snake
				// pong_init(); // TODO: countdown
				snake_state = SNAKE_PLAYING;
				break;
			case GAME_OVER:
				// TODO: hacer algun tipo de animacion con la pantalla de game over, mostrarla y morir.
				break;
			}
		}
		break;
	case SNAKE_GAME_OVER:

		break;

	}


	/*system("cls");
	printf( "Game over!"			// game over
			"\nBetter luck next time."
			"\n(Press C to continue...)");*/
	/*char key;
	do {
		key = getch();
		if(key == ESCAPE_KEY)
			exit(0);
	} while(key!='c' && key!='C');
	system("cls");*/

/*

}

void lose_a_life() {
	uint8_t i, touched_body_flag = 0;
	for(i = 4; i < snake.length; i++) {
		if(body[0].x == body[i].x && body[0].y == body[i].y) {
			touched_body_flag = 1;
			break;
		}
	}
	if(head.x <= 10 || head.x >= 10 + SNAKE_BOARD_WIDTH || head.y <= 5 || head.y >= 5 + SNAKE_BOARD_HEIGHT || touched_body_flag) {
		for(i = 0; i < snake.length-1; i++) {
			// Aca indico que toque de alguna forma (parpadeo de la viborita)
			/*
			MoveCursorToXY(body[i].x, body[i].y);
			printf("X");
			Sleep(80);
			printf("\b ");
			*/
		/*}
		// Aca indico la cantidad de vidas (hacerlo en el LCD)
		/*MoveCursorToXY(57, 4);
		printf("%hu", --snake.lives);*/

		/*
		if(!snake.lives) {
			game_on_flag_2 = 0; // me quede sin vidas, termina el juego
		}
		head = SPAWN_POINT;
		snake.bend_no = 0;
		for(i = 0; i < snake.length; i++) {
			body[i].x = body[i].y = 0;
		}
	}
}

void snake_move (void) {
	uint8_t i;
	uint8_t joystick;
	switch(head.direction) {
		case JOYSTICK_1_LEFT:
			for(i=0; i <= bend[snake.bend_no].x - head.x && snake.body_no < snake.length; i++, snake.body_no++) {
				body[snake.body_no].x = head.x + i;
				body[snake.body_no].y = head.y;
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.body_no].x, body[snake.body_no].y, MATRIX_LED_ON);
			}
			snake_bend();
			// si no se presiona nada, pero no puedo perder lo que sea que se haya presionado, hago un peek
			if(pdFALSE == xQueuePeek(joysticks_queue, &joystick, 0)){ // NO HACE NADA ES UN TACHO (voy a dejar este comentario 18/09/2023)
				head.x--;
			}
			break;
		case JOYSTICK_1_RIGHT:
			for(i=0; i <= head.x - bend[snake.bend_no].x && snake.body_no < snake.length; i++, snake.body_no++) {
				body[snake.body_no].x = head.x - i;
				body[snake.body_no].y = head.y;
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.body_no].x, body[snake.body_no].y, MATRIX_LED_ON);
			}
			snake_bend();
			if(pdFALSE == xQueuePeek(joysticks_queue, &joystick, 0)){
				head.x++;
			}
			break;
		case JOYSTICK_1_UP:
			for(i=0; i <= bend[snake.bend_no].y - head.y && snake.body_no < snake.length; i++, snake.body_no++) {
				body[snake.body_no].x = head.x;
				body[snake.body_no].y = head.y + i;
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.body_no].x, body[snake.body_no].y, MATRIX_LED_ON);
			}
			snake_bend();
			if(pdFALSE == xQueuePeek(joysticks_queue, &joystick, 0)){
				head.y--;
			}
			break;
		case JOYSTICK_1_DOWN:
			for(i=0; i <= head.y - bend[snake.bend_no].y && snake.body_no < snake.length; i++, snake.body_no++) {
				body[snake.body_no].x = head.x;
				body[snake.body_no].y = head.y - i;
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.body_no].x, body[snake.body_no].y, MATRIX_LED_ON);
			}
			snake_bend();
			if(pdFALSE == xQueuePeek(joysticks_queue, &joystick, 0)){
				head.y++;
			}
	}
}

void snake_bend() {
	MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.length-1].x, body[snake.length-1].y, MATRIX_LED_OFF);
	int16_t i, j, diff;
	for(i = snake.bend_no; i >= 0 && snake.body_no < snake.length; i--)
		if(bend[i].x == bend[i-1].x) {
			diff = bend[i].y - bend[i-1].y;
			for(j=1; j <= abs(diff) && snake.body_no < snake.length; j++, snake.body_no++) {
				body[snake.body_no].x = bend[i].x;
				body[snake.body_no].y = diff<0 ? bend[i].y + j : bend[i].y - j;
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.body_no].x, body[snake.body_no].y, MATRIX_LED_ON);
			}
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.length-1].x, body[snake.length-1].y, MATRIX_LED_OFF);
		} else if(bend[i].y == bend[i-1].y) {
			diff = bend[i].x - bend[i-1].x;
			for(j = 1; j <= abs(diff) && snake.body_no < snake.length; j++, snake.body_no++) {
				body[snake.body_no].x = diff<0 ? bend[i].x + j : bend[i].x - j;
				body[snake.body_no].y = bend[i].y;
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.body_no].x, body[snake.body_no].y, MATRIX_LED_ON);
			}
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.length-1].x, body[snake.length-1].y, MATRIX_LED_OFF);
		}
}

void snake_generate_food() {
	uint8_t snake_intersect_flag = 1;
	if(head.x == food.x && head.y == food.y) { // comio
		if(snake.length <= MAX_BODY_LENGTH) {
			snake.length++;
		}
		// MoveCursorToXY(27, 4);
		// printf("%u", ++snake.score); // aca printea el puntaje, esto tendria que hacerlo en el LCD
		snake.score++;
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, food.x, food.y, MATRIX_LED_OFF); // erase the eaten food from board
		while(snake_intersect_flag) {
			food.x = my_rand()%(SNAKE_BOARD_WIDTH - 1) + 11;
			food.y = my_rand()%(SNAKE_BOARD_HEIGHT - 1) + 6;
			for(uint8_t i=0; i<snake.length; i++) {
				if(food.x == body[i].x && food.y == body[i].y) {
					break;
				} else if (i == snake.length - 1) {
					snake_intersect_flag = 0;
				}
			}
		}
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, food.x, food.y, MATRIX_LED_ON); // shows the food on board with "F" (TODO: hacerla titilar para distinguirla?)
	} else if(food.x == 0 && food.y == 0) {			// generates food for the first time
		while(snake_intersect_flag) {
			food.x = my_rand()%(SNAKE_BOARD_WIDTH - 1) + 11;
			food.y = my_rand()%(SNAKE_BOARD_HEIGHT - 1) + 6;
			for(uint8_t i=0; i<snake.length; i++) {
				if(food.x == body[i].x && food.y == body[i].y) {
					break;
				} else if(i == snake.length - 1) {
					snake_intersect_flag = 0;
				}
			}
		}
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, food.x, food.y, MATRIX_LED_ON); // shows the food on board with "F" (TODO: hacerla titilar para distinguirla?)
	}
}*/
