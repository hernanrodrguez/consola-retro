/*
 * conway.c
 *
 *  Created on: 14 oct. 2023
 *      Author: hernan
 */

#include "conway.h"
#include "main.h"
#include "menu.h"
#include "joystick.h"
#include "DOT_MATRIX.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "buzzer.h"

uint32_t actual[32];
uint32_t futuro[32];
uint8_t last_pattern;

extern QueueHandle_t joysticks_queue;
extern QueueHandle_t game_queue;
extern QueueHandle_t buzzer_queue;

void conway_init(uint8_t pattern){

	MATRIX_clear_buffer(MATRIX_DISPLAY_UNIT1);
	MATRIX_CLEAR(MATRIX_DISPLAY_UNIT1);

	last_pattern = pattern;

	switch(pattern){
	case CONWAY_DIAMOND4812:
		for(uint8_t i=0; i<11; i++){
			actual[i] = 0;
		}
		actual[11] = 0b00000000000000111100000000000000;
		actual[12] = 0b00000000000000000000000000000000;
		actual[13] = 0b00000000000011111111000000000000;
		actual[14] = 0b00000000000000000000000000000000;
		actual[15] = 0b00000000001111111111110000000000;
		actual[16] = 0b00000000000000000000000000000000;
		actual[17] = 0b00000000000011111111000000000000;
		actual[18] = 0b00000000000000000000000000000000;
		actual[19] = 0b00000000000000111100000000000000;
		for(uint8_t i=20; i<32; i++){
			actual[i] = 0;
		}
		break;
	case CONWAY_GLIDER4:
		for(uint8_t i=0; i<11; i++){
			actual[i] = 0;
		}
		actual[11] = 0b00000000001000000000000000000000;
		actual[12] = 0b00000000001000000000000000000000;
		actual[13] = 0b00000000111000000000000000000000;
		actual[14] = 0b00000000000000000000000000000000;
		actual[15] = 0b00000000000000111000000000000000;
		actual[16] = 0b00000000000000010000000000000000;
		actual[17] = 0b00000000000000000000111000000000;
		actual[18] = 0b00000000000000000000100000000000;
		actual[19] = 0b00000000000000000000100000000000;
		for(uint8_t i=20; i<32; i++){
			actual[i] = 0;
		}
		break;
	case CONWAY_JASONP22:
		for(uint8_t i=0; i<11; i++){
			actual[i] = 0;
		}
		actual[11] = 0b00011000000000000000000000000000;
		actual[12] = 0b00001000000000000000000000000000;
		actual[13] = 0b00001010000000000000111000000000;
		actual[14] = 0b00000110001000000001000100000000;
		actual[15] = 0b00000000010110000001000010000000;
		actual[16] = 0b00000000100001000000110100000000;
		actual[17] = 0b00000000010001000000001000110000;
		actual[18] = 0b00000000001110000000000000101000;
		actual[19] = 0b00000000000000000000000000001000;
		actual[20] = 0b00000000000000000000000000001100;
		for(uint8_t i=21; i<32; i++){
			actual[i] = 0;
		}
		break;
	case CONWAY_RANDOM:
		for(uint8_t i=0; i<32; i++){
			actual[i] = (my_rand()%8)<<(my_rand()%24);
		}
		break;
	}

}

void conway_play(void) {

	typedef enum{
		CONWAY_FIRST_PRINT,
		CONWAY_COUNTDOWN,
		CONWAY_PLAYING,
		CONWAY_PAUSE,
		CONWAY_GAME_OVER
	} conway_state_t;

	uint8_t joystick, game_data, buzzer_data;
	static conway_state_t conway_state = CONWAY_FIRST_PRINT;

	switch(conway_state){
	case CONWAY_FIRST_PRINT:
		conway_state = CONWAY_COUNTDOWN;
		break;

	case CONWAY_COUNTDOWN:
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
				conway_state = CONWAY_PLAYING;
				break;
			}
		}
		break;

	case CONWAY_PLAYING:
		if(pdTRUE != xQueueReceive(joysticks_queue, &joystick, 0)) {
			conway_print();
			conway_evolve();
		} else if(joystick == JOYSTICK_1_PULS) {
			BUZZER_TONE();
			game_data = PLAYER_1_PAUSE;
			xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
			conway_state = CONWAY_PAUSE;
			taskYIELD();
		}
		break;

	case CONWAY_PAUSE:
		if(pdTRUE == xQueueReceive(game_queue, &game_data, 0)){
			switch(game_data){
			case PLAYER_1_PAUSE:
				game_data = PLAYER_1_PAUSE;
				xQueueSendToBack(game_queue, &game_data, portMAX_DELAY);
				taskYIELD();
				break;
			case GAME_COUNTDOWN:
				conway_state = CONWAY_COUNTDOWN;
				break;
			case GAME_RESUME:
				conway_state = CONWAY_PLAYING;
				break;
			case GAME_RESET:
				conway_init(last_pattern);
				conway_state = CONWAY_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			case GAME_OVER:
				MATRIX_print_msg(MATRIX_DISPLAY_UNIT1, GAMEOVER_MSG);
				conway_state = CONWAY_FIRST_PRINT;
				vTaskDelete(NULL);
				break;
			}
		}
		break;

	case CONWAY_GAME_OVER:
		MATRIX_print_msg(MATRIX_DISPLAY_UNIT1, GAMEOVER_MSG);
		conway_state = CONWAY_FIRST_PRINT;
		vTaskDelete(NULL);
		break;
	}



}

void set_cell_state(uint32_t *matrix, uint8_t i, uint8_t j, uint8_t state){
	uint32_t mask = 1 << j;
	if(state){
		matrix[i] |= mask;
	} else {
		mask = ~mask;
		matrix[i] &= mask;
	}
}

uint8_t get_cell_state(uint32_t row, uint8_t col){
	uint32_t mask = 1 << col;
	uint32_t cell = mask & row;
	return cell ? 1 : 0;
}

uint8_t alive_neighbours(uint32_t *matrix, uint8_t i, uint8_t j){
	uint8_t n=0;
	for(int8_t ii=i-1; ii<i+2; ii++){
		for(int8_t jj=j-1; jj<j+2; jj++){
			if(ii==i && jj==j) {
				jj++;
			}
			n += get_cell_state(matrix[(32+ii)%32], (32+jj)%32);
		}
	}
	return n;
}

void copy_matrix(uint32_t *dest, uint32_t *src){
	for(uint8_t i=0; i<32; i++) {
		dest[i] = src[i];
	}
}

void conway_evolve(void){
	for(uint8_t i=0; i<32; i++){
		for(uint8_t j=0; j<32; j++){
			uint8_t n = alive_neighbours(actual, i, j);
			if(!get_cell_state(actual[i], j) && n==3) {
				set_cell_state(futuro, i, j, 1);
			} else if(get_cell_state(actual[i], j) && (n==2 || n==3)) {
				set_cell_state(futuro, i, j, 1);
			} else {
				set_cell_state(futuro, i, j, 0);
			}
		}
	}
	copy_matrix(actual, futuro);
}

void init_conway(void){
	for(uint8_t i=0; i<32; i++){
		actual[i] = my_rand();
	}
}

void conway_print(void) {
	uint8_t state;
	for(uint8_t i=0; i<32; i++){
		for(uint8_t j=0; j<32; j++){
			state = get_cell_state(actual[i], j);
			MATRIX_set_led(MATRIX_DISPLAY_UNIT1, i, j, state);
		}
	}
	MATRIX_display_buffer(MATRIX_DISPLAY_UNIT1);
}
