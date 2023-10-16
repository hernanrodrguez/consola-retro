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
#define SINGLE_PLAYER_POINT 7
#define SINGLE_PLAYER_LIVE  8
#define SINGLE_PLAYER_PAUSE 9
#define SINGLE_PLAYER_WIN   10
#define SINGLE_PLAYER_LOST	11
#define GAME_START			12
#define GAME_RESUME			13
#define GAME_RESET			14
#define GAME_OVER			15
#define SEC_3				16
#define SEC_2				17
#define SEC_1				18
#define SEC_0				19
#define GAME_COUNTDOWN 		20
#define TETRIS_LINE 		21

#define LINES_RULES_PONG 	6
#define LINES_RULES_SNAKE 	5
#define LINES_RULES_TETRIS 	6
#define LINES_RULES_CONWAY 	8

#define RESET_GAME_STATE() seconds=0; minutes=0; score_1=0; score_2=0; score=0; lives=3

#define BUZZER_TONE() do { \
    buzzer_data = NAVIGATION_TONE; \
    xQueueSendToBack(buzzer_queue, &buzzer_data, 0); \
} while (0)

#define BUZZER_LONG_TONE() do { \
    buzzer_data = LONG_TONE; \
    xQueueSendToBack(buzzer_queue, &buzzer_data, 0); \
} while (0)

#define BUZZER_POINT_TONE() do { \
    buzzer_data = POINT_TONE; \
    xQueueSendToBack(buzzer_queue, &buzzer_data, 0); \
} while (0)

#define BUZZER_LIVE_TONE() do { \
    buzzer_data = LIVE_TONE; \
    xQueueSendToBack(buzzer_queue, &buzzer_data, 0); \
} while (0)

#define BUZZER_VICTORY_TONE() do { \
    buzzer_data = VICTORY_TONE; \
    xQueueSendToBack(buzzer_queue, &buzzer_data, 0); \
} while (0)

#define BUZZER_LOST_TONE() do { \
    buzzer_data = LOST_TONE; \
    xQueueSendToBack(buzzer_queue, &buzzer_data, 0); \
} while (0)



uint8_t menu_handle(void);
uint8_t menu_game_handle(void);
uint8_t menu_pause_handle(void);
uint8_t menu_gameover_handle(void);
uint8_t menu_game_play(uint8_t game, const char *text);
uint8_t menu_conway_options_handle(void);
uint8_t menu_rules_handle (uint8_t game);
void menu_blink(uint8_t option, const char *text);
void menu_blink_option(uint8_t option, const char *text);

#endif /* INC_MENU_H_ */
