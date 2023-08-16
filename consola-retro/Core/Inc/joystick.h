/*
 * joystick.h
 *
 *  Created on: 16 ago. 2023
 *      Author: hernan
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include <stdint.h>

#define NONE 				0

#define JOYSTICK_1_RIGHT 	1
#define JOYSTICK_1_LEFT 	2
#define JOYSTICK_1_UP 		3
#define JOYSTICK_1_DOWN 	4
#define JOYSTICK_1_PULS		5

#define JOYSTICK_2_RIGHT 	6
#define JOYSTICK_2_LEFT 	7
#define JOYSTICK_2_UP 		8
#define JOYSTICK_2_DOWN 	9
#define JOYSTICK_2_PULS		10

void init_joysticks(void);
uint8_t read_joysticks(void);

#endif /* INC_JOYSTICK_H_ */
