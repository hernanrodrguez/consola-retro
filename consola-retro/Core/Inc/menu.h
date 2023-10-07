/*
 * menu.h
 *
 *  Created on: 19 ago. 2023
 *      Author: hernan
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include <stdint.h>

#define BLINK_RATE 300
#define ONE_SECOND 1000

#define PLAYER_1_POINT 		1
#define PLAYER_2_POINT 		2
#define PLAYER_1_PAUSE 		3
#define PLAYER_2_PAUSE 		4
#define PLAYER_1_WIN    	5
#define PLAYER_2_WIN 		6
#define SINGLE_PLAYER_WIN   7
#define SINGLE_PLAYER_LOST	8
#define GAME_RESUME			9
#define GAME_RESET			10
#define GAME_OVER			11

uint8_t menu_handle(void);
uint8_t menu_game_handle(void);
uint8_t menu_pause_handle(void);
uint8_t menu_gameover_handle(void);
uint8_t menu_game_0_play(void);
void menu_blink(uint8_t option, const char *text);
void menu_blink_option(uint8_t option, const char *text);

#endif /* INC_MENU_H_ */
