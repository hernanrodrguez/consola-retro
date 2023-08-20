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

uint8_t menu_handle(void);
uint8_t menu_game_handle(void);
uint8_t menu_game_play(void);
void menu_blink(uint8_t option, const char *text);
void menu_blink_option(uint8_t option, const char *text);

#endif /* INC_MENU_H_ */
