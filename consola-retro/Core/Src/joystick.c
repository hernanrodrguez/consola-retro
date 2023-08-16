/*
 * joystick.c
 *
 *  Created on: 16 ago. 2023
 *      Author: hernan
 */

#include "joystick.h"
#include "debounce.h"
#include "main.h"

debounce_t deb_right_1;
debounce_t deb_left_1;
debounce_t deb_up_1;
debounce_t deb_down_1;

debounce_t deb_right_2;
debounce_t deb_left_2;
debounce_t deb_up_2;
debounce_t deb_down_2;

void init_joysticks(void){
	debounce_init(&deb_right_1, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_left_1, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_up_1, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_down_1, 1, DEBOUNCE_TICKS);

	debounce_init(&deb_right_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_left_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_up_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_down_2, 1, DEBOUNCE_TICKS);
}

uint8_t read_joysticks(void){ // por ahora no contemplo el caso del pulsador para encararlo

	debounce_check(&deb_right_1, HAL_GPIO_ReadPin(EJE_X_UP_1_GPIO_Port, EJE_X_UP_1_Pin));
	if(debounce_edge(&deb_right_1)){
		return JOYSTICK_1_RIGHT;
	}

	debounce_check(&deb_left_1, HAL_GPIO_ReadPin(EJE_X_DOWN_1_GPIO_Port, EJE_X_DOWN_1_Pin));
	if(debounce_edge(&deb_left_1)){
		return JOYSTICK_1_LEFT;
	}

	debounce_check(&deb_up_1, HAL_GPIO_ReadPin(EJE_Y_UP_1_GPIO_Port, EJE_Y_UP_1_Pin));
	if(debounce_edge(&deb_up_1)){
		return JOYSTICK_1_UP;
	}

	debounce_check(&deb_down_1, HAL_GPIO_ReadPin(EJE_Y_DOWN_1_GPIO_Port, EJE_Y_DOWN_1_Pin));
	if(debounce_edge(&deb_down_1)){
		return JOYSTICK_1_DOWN;
	}

	debounce_check(&deb_right_2, HAL_GPIO_ReadPin(EJE_X_UP_2_GPIO_Port, EJE_X_UP_2_Pin));
	if(debounce_edge(&deb_right_2)){
		return JOYSTICK_2_RIGHT;
	}

	debounce_check(&deb_left_2, HAL_GPIO_ReadPin(EJE_X_DOWN_2_GPIO_Port, EJE_X_DOWN_2_Pin));
	if(debounce_edge(&deb_left_2)){
		return JOYSTICK_2_LEFT;
	}

	debounce_check(&deb_up_2, HAL_GPIO_ReadPin(EJE_Y_UP_2_GPIO_Port, EJE_Y_UP_2_Pin));
	if(debounce_edge(&deb_up_2)){
		return JOYSTICK_2_UP;
	}

	debounce_check(&deb_down_2, HAL_GPIO_ReadPin(EJE_Y_DOWN_2_GPIO_Port, EJE_Y_DOWN_2_Pin));
	if(debounce_edge(&deb_down_2)){
		return JOYSTICK_2_DOWN;
	}



}
