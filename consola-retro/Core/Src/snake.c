/*
 * snake.c
 *
 *  Created on: 18 sep. 2023
 *      Author: hernan
 */

#include "snake.h"
#include "main.h"
#include "menu.h"
#include "joystick.h"
#include "DOT_MATRIX.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "buzzer.h"

const point_t SPAWN_POINT = (point_t){15, 15, JOYSTICK_1_RIGHT};

point_t head, bend[500], food, body[MAX_BODY_LENGTH+1];
snake_t snake = (snake_t){0, 0, DEFAULT_BODY_LENGTH + 1, SNAKE_DEFAULT_LIVES, 0};

extern QueueHandle_t joysticks_queue;
extern QueueHandle_t game_queue;
extern QueueHandle_t buzzer_queue;

static uint32_t my_rand(void){
	uint32_t y = HAL_GetTick();
	y^= y<<13;
	y^= y>>17;
	y^= y<<5;
	return y;
}

void snake_init(void) {
	food = (point_t){0, 0, 0};
	bend[0] = head = SPAWN_POINT;
	snake = (snake_t){0, 0, DEFAULT_BODY_LENGTH + 1, SNAKE_DEFAULT_LIVES, 0};

	MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
	MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
}

void snake_play(void) {
	typedef enum{
		SNAKE_FIRST_PRINT,
		SNAKE_COUNTDOWN,
		SNAKE_PLAYING,
		SNAKE_PAUSE,
		SNAKE_GAME_OVER
	} snake_state_t;

	uint8_t joystick, game_data, buzzer_data;
	static snake_state_t snake_state = SNAKE_FIRST_PRINT;

	switch(snake_state){
	case SNAKE_FIRST_PRINT:
		snake_init();
		snake_state = SNAKE_COUNTDOWN;
		break;
	case SNAKE_COUNTDOWN:
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
				snake_state = SNAKE_PLAYING;
				break;
			}
		}
		break;
	case SNAKE_PLAYING:
		snake_generate_food();
		snake.body_no = 0;
		snake_move();
		if(lose_a_life() != SINGLE_PLAYER_LOST) {
			if(pdTRUE == xQueueReceive(joysticks_queue, &joystick, 0)){
				switch(joystick) {
					case JOYSTICK_1_LEFT:
					case JOYSTICK_1_RIGHT:
						switch(head.direction) {
							case JOYSTICK_1_UP:
							case JOYSTICK_1_DOWN:
								bend[++snake.bend_no] = head;
								head.direction = joystick;
								switch(joystick) {
									case JOYSTICK_1_LEFT:
										head.x--;
										break;
									case JOYSTICK_1_RIGHT:
										head.x++;
										break;
									case JOYSTICK_1_UP:
										head.y++;
										break;
									case JOYSTICK_1_DOWN:
										head.y--;
										break;
								}
						}
						break;
					case JOYSTICK_1_UP:
					case JOYSTICK_1_DOWN:
						switch(head.direction) {
							case JOYSTICK_1_LEFT:
							case JOYSTICK_1_RIGHT:
								bend[++snake.bend_no] = head;
								head.direction = joystick;
								switch(joystick) {
									case JOYSTICK_1_LEFT:
										head.x--;
										break;
									case JOYSTICK_1_RIGHT:
										head.x++;
										break;
									case JOYSTICK_1_UP:
										head.y++;
										break;
									case JOYSTICK_1_DOWN:
										head.y--;
										break;
								}
						}
						break;
					case JOYSTICK_1_PULS: // pausa
						BUZZER_TONE();
						game_data = PLAYER_1_PAUSE;
						xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
						snake_state = SNAKE_PAUSE;
						break;
				}
			}
			MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		} else {
			snake_state = SNAKE_GAME_OVER;
		}
		break;
	case SNAKE_PAUSE:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case PLAYER_1_PAUSE:
				game_data = PLAYER_1_PAUSE;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				taskYIELD();
				break;
			case GAME_COUNTDOWN:
				snake_state = SNAKE_COUNTDOWN;
				break;
			case GAME_RESUME:
				snake_state = SNAKE_PLAYING;
				break;
			case GAME_RESET:
				snake_state = SNAKE_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			case GAME_OVER:
				MATRIX_print_msg(MATRIX_DISPLAY_UNIT1, GAMEOVER_MSG);
				snake_state = SNAKE_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			}
		}
		break;
	case SNAKE_GAME_OVER:
		MATRIX_print_msg(MATRIX_DISPLAY_UNIT1, GAMEOVER_MSG);
		snake_state = SNAKE_FIRST_PRINT;
		vTaskDelete(NULL);
		break;
	}
}

uint8_t lose_a_life(void) {
	uint8_t game_data, buzzer_data;
	uint8_t i, touched_body_flag = 0;
	for(i = 4; i < snake.length; i++) {
		if(body[0].x == body[i].x && body[0].y == body[i].y) {
			touched_body_flag = 1;
			break;
		}
	}
	if(head.x < 0 || head.x > SNAKE_BOARD_WIDTH || head.y < 0 || head.y > SNAKE_BOARD_HEIGHT || touched_body_flag) {
		snake.lives--;

		if(!snake.lives) {
			BUZZER_LOST_TONE();
			game_data = SINGLE_PLAYER_LOST;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
			snake_blink();
			return SINGLE_PLAYER_LOST;
		}

		BUZZER_LIVE_TONE();
		game_data = SINGLE_PLAYER_LIVE;
		xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
		snake_blink();
		head = SPAWN_POINT;
		snake.bend_no = 0;
		for(i = 0; i < snake.length; i++) {
			body[i].x = body[i].y = 0;
		}
	}
	return 0;
}

void snake_blink(void){
	for(uint8_t j=0; j<3; j++){
		for(uint8_t i=0; i < snake.length-1; i++) {
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[i].x, body[i].y, MATRIX_LED_OFF);
		}
		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		vTaskDelay(10/portTICK_PERIOD_MS);
		for(uint8_t i=0; i < snake.length-1; i++) {
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[i].x, body[i].y, MATRIX_LED_ON);
		}
		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
	for(uint8_t i=0; i < snake.length-1; i++) {
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[i].x, body[i].y, MATRIX_LED_OFF);
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
			break;
	}
}

void snake_bend(void) {
	int16_t i, j, diff;

	MATRIX_set_led(MATRIX_DISPLAY_UNIT1, body[snake.length-1].x, body[snake.length-1].y, MATRIX_LED_OFF);

	for(i = snake.bend_no; i >= 0 && snake.body_no < snake.length; i--) {
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
}

void snake_generate_food() {
	uint8_t snake_intersect_flag = 1;
	uint8_t game_data, buzzer_data;
	if(head.x == food.x && head.y == food.y) { // comio
		if(snake.length <= MAX_BODY_LENGTH) {
			snake.length++;
		}

		BUZZER_POINT_TONE();
		game_data = SINGLE_PLAYER_POINT;
		xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);

		snake.score++;

		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, food.x, food.y, MATRIX_LED_OFF); // erase the eaten food from board
		while(snake_intersect_flag) {
			food.x = (my_rand()%28) + 2; // nro aleatorio del 2 al 29
			food.y = (my_rand()%28) + 2; // nro aleatorio del 2 al 29
			for(uint8_t i=0; i<snake.length; i++) {
				if(food.x == body[i].x && food.y == body[i].y) {
					break;
				} else if (i == snake.length - 1) {
					snake_intersect_flag = 0;
				}
			}
		}
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, food.x, food.y, MATRIX_LED_ON);
	} else if(food.x == 0 && food.y == 0) {	// genera comida por primera vez
		while(snake_intersect_flag) {
			food.x = (my_rand()%28) + 2; // nro aleatorio del 2 al 29
			food.y = (my_rand()%28) + 2; // nro aleatorio del 2 al 29
			for(uint8_t i=0; i<snake.length; i++) {
				if(food.x == body[i].x && food.y == body[i].y) {
					break;
				} else if(i == snake.length - 1) {
					snake_intersect_flag = 0;
				}
			}
		}
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, food.x, food.y, MATRIX_LED_ON);
	}
}
