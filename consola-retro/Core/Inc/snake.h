/*
 * snake.h
 *
 *  Created on: 18 sep. 2023
 *      Author: hernan
 */

#ifndef INC_SNAKE_H_
#define INC_SNAKE_H_

#include <stdint.h>
#include <stdlib.h>

#define SNAKE_DEFAULT_LIVES 3
#define DEFAULT_BODY_LENGTH 5		// the initial length of the snake is 5, and it can reach a maximum length of 29
#define MAX_BODY_LENGTH 29
#define SNAKE_BOARD_WIDTH 60
#define SNAKE_BOARD_HEIGHT 20

// point struct definition
typedef struct {
	uint8_t x, y;	// x and y coordinates of the point
	uint8_t direction;			// the direction where the point is headed (relevant if the point is on the snake)
} point_t;

// Snake struct definition
typedef struct {
	uint8_t bend_no, body_no, length, lives;		// length and lives of the snake
	uint32_t score;		// score
} snake_t;

uint8_t snake_play(void);

void lose_a_life(void);

void snake_move (void);

void snake_bend(void);

void snake_generate_food(void);


#endif /* INC_SNAKE_H_ */
