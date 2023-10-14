/*
 * tetris.h
 *
 *  Created on: 13 oct. 2023
 *      Author: hernan
 */

#ifndef INC_TETRIS_H_
#define INC_TETRIS_H_

#include <stdint.h>

#define TETRIS_BOARD_WIDTH  16
#define TETRIS_BOARD_HEIGHT 32

#define DEFAULT_TIMER 500000

typedef struct {
	char **shape_matrix; // stores the shape of the tetromino in a char matrix
	uint8_t width;		 // width of the tetromino
	int8_t x, y;		 // x and y coordinates of the location of tetromino
} shape_t;

typedef struct {
	int8_t board[TETRIS_BOARD_HEIGHT][TETRIS_BOARD_WIDTH];	// game board
	uint32_t score, timer;								// score and timer
} tetris_t;


void tetris_play(void);

void tetris_init(void);

//void control_current_shape(uint8_t joystick);

shape_t* copy_shape(const shape_t shape);

void erase_shape_from_console(void);

uint8_t check_position(shape_t *shape);

void write_shape_to_board(void);

void check_rows(void);

uint8_t get_new_shape(void);

void rotate_shape(shape_t *shape);

void delete_shape(shape_t *shape);

void print_shape_to_console(void);

void delete_shape_from_board(void);

#endif /* INC_TETRIS_H_ */
