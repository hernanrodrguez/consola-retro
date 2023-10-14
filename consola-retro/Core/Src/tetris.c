/*
 * tetris.c
 *
 *  Created on: 13 oct. 2023
 *      Author: hernan
 */

#include "tetris.h"
#include "main.h"
#include "menu.h"
#include "joystick.h"
#include "DOT_MATRIX.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "buzzer.h"

const shape_t tetrominos[7] = {
	{(char *[]){(char []){0,0,0,0},
				(char []){1,1,1,1},
				(char []){0,0,0,0},
				(char []){0,0,0,0}}, 4, 0, 0},		// I tetromino
	{(char *[]){(char []){1,1},
				(char []){1,1}}, 2, 0, 0},			// O tetromino
	{(char *[]){(char []){0,1,0},
				(char []){1,1,1},
				(char []){0,0,0}}, 3, 0, 0},		// T tetromino
	{(char *[]){(char []){0,1,1},
				(char []){1,1,0},
				(char []){0,0,0}}, 3, 0, 0},		// S tetromino
	{(char *[]){(char []){1,1,0},
				(char []){0,1,1},
				(char []){0,0,0}}, 3, 0, 0},		// Z tetromino
	{(char *[]){(char []){0,0,1},
				(char []){1,1,1},
				(char []){0,0,0}}, 3, 0, 0},		// J tetromino
	{(char *[]){(char []){1,0,0},
				(char []){1,1,1},
				(char []){0,0,0}}, 3, 0, 0}			// L tetromino
};

shape_t *current;
tetris_t tetris = (tetris_t){{{0}}, 0, DEFAULT_TIMER};

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

void tetris_init(void) {
	tetris = (tetris_t){{{0}}, 0, DEFAULT_TIMER};
	get_new_shape();

	MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);

	for(uint8_t i=0; i<32; i++){
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 7, i, MATRIX_LED_ON);
		MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 24, i, MATRIX_LED_ON);
	}

	MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
}

void tetris_play(void) {

	typedef enum{
		TETRIS_FIRST_PRINT,
		TETRIS_COUNTDOWN,
		TETRIS_PLAYING,
		TETRIS_PAUSE,
		TETRIS_GAME_OVER
	} tetris_state_t;

	shape_t *temp;
	uint8_t joystick, game_data, buzzer_data;
	static tetris_state_t tetris_state = TETRIS_FIRST_PRINT;

	switch(tetris_state) {
	case TETRIS_FIRST_PRINT:
		tetris_init();
		tetris_state = TETRIS_COUNTDOWN;
		break;

	case TETRIS_COUNTDOWN:
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
				tetris_state = TETRIS_PLAYING;
				break;
			}
		}
		break;

	case TETRIS_PLAYING:
		if(pdTRUE != xQueueReceive(joysticks_queue, &joystick, 0)) {
			joystick = JOYSTICK_1_DOWN; // si no se apreto nada, se mueve para abajo
		}

		temp = copy_shape(*current);
		switch(joystick) {
			case JOYSTICK_1_LEFT: // mover pieza a la izquierda
				erase_shape_from_console();
				temp->x--;
				if(check_position(temp)) {
					current->x--;
				} else {
					BUZZER_TONE();
				}
				break;
			case JOYSTICK_1_RIGHT: // mover pieza a la derecha
				erase_shape_from_console();
				temp->x++;
				if(check_position(temp)) {
					current->x++;
				} else {
					BUZZER_TONE();
				}
				break;
			case JOYSTICK_1_DOWN: // mover pieza hacia abajo
				erase_shape_from_console();
				temp->y++;
				if(check_position(temp)) {
					current->y++;
				} else {
					write_shape_to_board();
					check_rows(); // after placing the shape, check if the any rows are filled
					if(get_new_shape()) {
						BUZZER_LOST_TONE();
						game_data = SINGLE_PLAYER_LOST;
						xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
						tetris_state = TETRIS_GAME_OVER;
					}
				}
				break;
			case JOYSTICK_1_UP:	// rotar pieza
				erase_shape_from_console();
				rotate_shape(temp);
				if(check_position(temp)) {
					rotate_shape(current);
					BUZZER_TONE();
				}
				break;
			case JOYSTICK_1_PULS: // pausa
				BUZZER_TONE();

				game_data = PLAYER_1_PAUSE;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				tetris_state = TETRIS_PAUSE;
				taskYIELD();
				break;
		}
		delete_shape(temp);
		write_shape_to_board();
		print_shape_to_console();
		delete_shape_from_board();
		MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
		break;

	case TETRIS_PAUSE:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case PLAYER_1_PAUSE:
				game_data = PLAYER_1_PAUSE;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				taskYIELD();
				break;
			case GAME_COUNTDOWN:
				tetris_state = TETRIS_COUNTDOWN;
				break;
			case GAME_RESUME:
				tetris_state = TETRIS_PLAYING; // TODO: countdown
				break;
			case GAME_RESET:
				tetris_state = TETRIS_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			case GAME_OVER:
				// TODO: hacer algun tipo de animacion con la pantalla de game over, mostrarla y morir.
				tetris_state = TETRIS_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			}
		}
		break;

	case TETRIS_GAME_OVER:
		// TODO: mostrar alguna animacion
		//MATRIX_print_game_over(MATRIX_DISPLAY_UNIT1);
		tetris_state = TETRIS_FIRST_PRINT;
		vTaskDelete(NULL); // chau chau adios...
		break;
	}
}

shape_t* copy_shape(const shape_t shape) {
	uint8_t i, j;
	shape_t *copy = (shape_t*) malloc(sizeof(shape_t));

	copy->width = shape.width;
	copy->y = shape.y;
	copy->x = shape.x;
	copy->shape_matrix = (char**) malloc(copy->width * sizeof(char*));

	for(i = 0; i < copy->width; i++) {
		copy->shape_matrix[i] = (char*) malloc(copy->width * sizeof(char));
		for(j = 0; j < copy->width; j++) {
			copy->shape_matrix[i][j] = shape.shape_matrix[i][j];
		}
	}
	return copy;
}

void erase_shape_from_console(void) {
	uint8_t i, j;
	for(i=0; i<current->width; i++) {
		for(j=0; j<current->width; j++) {
			if(current->shape_matrix[i][j]) {
				//MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 31 + 2*(j + current->x), 4 + i + current->y, MATRIX_LED_OFF);
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 8+j+current->x, i+current->y, MATRIX_LED_OFF);
				// display buffer?
			}
		}
	}
}

uint8_t check_position(shape_t *shape) {
	uint8_t i, j;
	for(i=0; i<shape->width; i++) {
		for(j=0; j<shape->width; j++) {
			if(shape->shape_matrix[i][j]) {
				if(shape->x + j < 0 || shape->x + j >= TETRIS_BOARD_WIDTH || shape->y + i >= TETRIS_BOARD_HEIGHT) {	// pieza fuera de los limites
					return 0;
				} else if(tetris.board[shape->y + i][shape->x + j]) { // otra pieza ocupa esa posicion
					return 0;
				}
			}
		}
	}
	return 1;
}

void write_shape_to_board() {
	uint8_t i, j;
	for(i=0; i<current->width; i++) {
		for(j=0; j<current->width; j++) {
			if(current->shape_matrix[i][j]) {
				tetris.board[current->y + i][current->x + j] = 1;
			}
		}
	}
}

void check_rows(void) {
	uint8_t i, j, game_data, buzzer_data, counter = 0;
	for(i=0; i<TETRIS_BOARD_HEIGHT; i++) {
		uint8_t sum = 0;
		for(j=0; j<TETRIS_BOARD_WIDTH; j++){
			sum += tetris.board[i][j];
		}
		if(sum == TETRIS_BOARD_WIDTH) {
			counter++;
			for(j=0; j<TETRIS_BOARD_WIDTH; j++) {
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 8+j, i, MATRIX_LED_OFF);
			}
			uint8_t k;
			for(k=i; k>=1; k--) {
				for(j=0; j<TETRIS_BOARD_WIDTH; j++) {
					if(!tetris.board[k-1][j] && tetris.board[k][j]) {
						MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 8+j, k, MATRIX_LED_OFF);
					}
					tetris.board[k][j] = tetris.board[k-1][j];
				}
			}
			for(j=0;j<TETRIS_BOARD_WIDTH;j++) {
				tetris.board[k][j] = 0;
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 8+j, k, MATRIX_LED_OFF);
			}
			BUZZER_POINT_TONE();
		} else {
			BUZZER_TONE();
		}
	}
	tetris.timer -= 1000;
	tetris.score += 100*counter;

	game_data = SINGLE_PLAYER_POINT;
	xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
}

uint8_t get_new_shape(void) {
	current = copy_shape(tetrominos[my_rand()%7]);
	current->x = my_rand()%(TETRIS_BOARD_WIDTH - current->width + 1);
	if(!check_position(current)) {
		return 1; // termino la partida
	}
	return 0;
}

void rotate_shape(shape_t *shape) {
	uint8_t i, j;
	shape_t *temp = copy_shape(*shape);

	for(i=0; i<shape->width; i++) {
		for(j=0;j<shape->width;j++) {
			shape->shape_matrix[i][j] = temp->shape_matrix[shape->width - j - 1][i];
		}
	}
	delete_shape(temp);
}

void delete_shape(shape_t *shape) {
	uint8_t i;
	for(i=0; i<shape->width; i++) {
		free(shape->shape_matrix[i]);
	}
	free(shape->shape_matrix);
	free(shape);
}

void print_shape_to_console(void) {
	uint8_t i, j;
	for(i=0; i<TETRIS_BOARD_HEIGHT; i++) {
		for(j=0; j<TETRIS_BOARD_WIDTH; j++) {
			if(tetris.board[i][j]) {
				//MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 31+2*j, 4+i, MATRIX_LED_ON);
				MATRIX_set_led(MATRIX_DISPLAY_UNIT1, 8+j, i, MATRIX_LED_ON);
			}
		}
	}
}

void delete_shape_from_board(void) {
	uint8_t i,j;
	for(i=0; i<current->width; i++) {
		for(j=0; j<current->width; j++) {
			if(current->shape_matrix[i][j]) {
				tetris.board[current->y + i][current->x + j] = 0;
			}
		}
	}
}
