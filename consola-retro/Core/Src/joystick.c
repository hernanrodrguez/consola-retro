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
debounce_t deb_puls_1;

debounce_t deb_right_2;
debounce_t deb_left_2;
debounce_t deb_up_2;
debounce_t deb_down_2;
debounce_t deb_puls_2;

void init_joysticks(void){
	debounce_init(&deb_right_1, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_left_1, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_up_1, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_down_1, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_puls_1, 1, DEBOUNCE_TICKS);

	debounce_init(&deb_right_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_left_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_up_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_down_2, 1, DEBOUNCE_TICKS);
	debounce_init(&deb_puls_2, 1, DEBOUNCE_TICKS);
}

uint8_t read_puls(uint8_t n_joystick){
	if(n_joystick == JOYSTICK_1){
		return( HAL_GPIO_ReadPin(EJE_X_UP_1_GPIO_Port, EJE_X_UP_1_Pin) &&
				HAL_GPIO_ReadPin(EJE_X_DOWN_1_GPIO_Port, EJE_X_DOWN_1_Pin) &&
				HAL_GPIO_ReadPin(EJE_Y_UP_1_GPIO_Port, EJE_Y_UP_1_Pin) &&
				HAL_GPIO_ReadPin(EJE_Y_DOWN_1_GPIO_Port, EJE_Y_DOWN_1_Pin));
	} else {
		return( HAL_GPIO_ReadPin(EJE_X_UP_2_GPIO_Port, EJE_X_UP_2_Pin) &&
				HAL_GPIO_ReadPin(EJE_X_DOWN_2_GPIO_Port, EJE_X_DOWN_2_Pin) &&
				HAL_GPIO_ReadPin(EJE_Y_UP_2_GPIO_Port, EJE_Y_UP_2_Pin) &&
				HAL_GPIO_ReadPin(EJE_Y_DOWN_2_GPIO_Port, EJE_Y_DOWN_2_Pin));
	}
}

uint8_t read_joysticks(void){

	debounce_check(&deb_right_1, HAL_GPIO_ReadPin(EJE_X_UP_1_GPIO_Port, EJE_X_UP_1_Pin));
	debounce_check(&deb_left_1, HAL_GPIO_ReadPin(EJE_X_DOWN_1_GPIO_Port, EJE_X_DOWN_1_Pin));
	debounce_check(&deb_up_1, HAL_GPIO_ReadPin(EJE_Y_UP_1_GPIO_Port, EJE_Y_UP_1_Pin));
	debounce_check(&deb_down_1, HAL_GPIO_ReadPin(EJE_Y_DOWN_1_GPIO_Port, EJE_Y_DOWN_1_Pin));
	debounce_check(&deb_puls_1, read_puls(JOYSTICK_1));

	debounce_check(&deb_right_2, HAL_GPIO_ReadPin(EJE_X_UP_2_GPIO_Port, EJE_X_UP_2_Pin));
	debounce_check(&deb_left_2, HAL_GPIO_ReadPin(EJE_X_DOWN_2_GPIO_Port, EJE_X_DOWN_2_Pin));
	debounce_check(&deb_up_2, HAL_GPIO_ReadPin(EJE_Y_UP_2_GPIO_Port, EJE_Y_UP_2_Pin));
	debounce_check(&deb_down_2, HAL_GPIO_ReadPin(EJE_Y_DOWN_2_GPIO_Port, EJE_Y_DOWN_2_Pin));
	debounce_check(&deb_puls_2, read_puls(JOYSTICK_2));

	if(debounce_edge(&deb_right_1)){
		return JOYSTICK_1_RIGHT;
	}

	if(debounce_edge(&deb_left_1)){
		return JOYSTICK_1_LEFT;
	}

	if(debounce_edge(&deb_up_1)){
		return JOYSTICK_1_UP;
	}

	if(debounce_edge(&deb_down_1)){
		return JOYSTICK_1_DOWN;
	}

	if(debounce_edge(&deb_puls_1)){
		return JOYSTICK_1_PULS;
	}

	if(debounce_edge(&deb_right_2)){
		return JOYSTICK_2_RIGHT;
	}


	if(debounce_edge(&deb_left_2)){
		return JOYSTICK_2_LEFT;
	}

	if(debounce_edge(&deb_up_2)){
		return JOYSTICK_2_UP;
	}

	if(debounce_edge(&deb_down_2)){
		return JOYSTICK_2_DOWN;
	}

	if(debounce_edge(&deb_puls_2)){
		return JOYSTICK_2_PULS;
	}

	return NONE;

}
