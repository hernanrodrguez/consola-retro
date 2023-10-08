/*
 * pong.h
 *
 *  Created on: 25 ago. 2023
 *      Author: hernan
 */

#ifndef INC_PONG_H_
#define INC_PONG_H_

#include <stdint.h>

#define PONG_BOARD_WIDTH 31
#define PONG_BOARD_HEIGHT 31
#define PADDLE_1_LENGTH 5
#define PADDLE_2_LENGTH 5
#define END_SCORE 5

#define NO_TOUCH	1
#define TOUCH 		0

typedef struct {
	uint8_t x, y, score;
} player_t;

typedef struct {
	uint8_t x, y, direction;
} ball_t;

typedef enum {
	RIGHT_UP, LEFT_UP, RIGHT_DOWN, LEFT_DOWN
} ball_direction_t;

void pong_init(void);
void pong_play(void);
void pong_print_board(void);
void pong_move_ball(uint8_t touch);
uint8_t pong_change_ball_direction(void);
void ball_blink(void);

#endif /* INC_PONG_H_ */
