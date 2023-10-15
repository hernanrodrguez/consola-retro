/*
 * buzzer.c
 *
 *  Created on: 18 sep. 2023
 *      Author: hernan
 */


#include "buzzer.h"
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"

note_t power_on_melody[] = {
    {784, 200},
    {988, 200},
    {1175, 200},
    {1397, 400},
    {1175, 200},
    {1397, 400},
    {1568, 400},
    {1760, 400},
};

note_t short_victory_melody[] = {
    {784, 400},
    {987, 400},
    {1175, 400},
    {1397, 800},
};

note_t long_victory_melody[] = {
    {784, 100},
    {932, 100},
    {1046, 100},
    {1174, 200},
    {1397, 200},
    {1568, 100},
    {1865, 100},
    {2093, 200},
    {2349, 200},
    {3136, 400},
};

note_t lost_life_melody[] = {
    {392, 200},
    {294, 200},
    {262, 200},
    {196, 400},
};

note_t melody[] = {
    {440, 500},
    {523, 500},
    {587, 500},
    {659, 500},
};

note_t navigation_tone[] = {
    {784, 100},
};

note_t long_tone[] = {
    {784, 500},
};

note_t lost_game_melody[] = {
    {392, 400},
    {294, 400},
    {262, 400},
    {196, 800},
};

extern QueueHandle_t buzzer_queue;
extern TIM_HandleTypeDef htim3;

/*void PlayTone(uint16_t frequency, uint16_t duration) {
    __HAL_TIM_SET_AUTORELOAD(&htim3, 1000000 / frequency); // Calcula el período en función de la frecuencia
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 50); // Duty cycle del 50% (puedes ajustarlo según sea necesario)
    HAL_Delay(duration);
}*/

void buzzer_play(void) {
	static uint8_t buzzer_data;

	if(pdTRUE == xQueueReceive(buzzer_queue, &buzzer_data, portMAX_DELAY)){
		switch(buzzer_data){
		case OPENING_MELODY:
			buzzer_play_melody(power_on_melody, (sizeof(power_on_melody)/sizeof(note_t)));
			break;
		case NAVIGATION_TONE:
			buzzer_play_melody(navigation_tone, (sizeof(navigation_tone)/sizeof(note_t)));
			break;
		case LONG_TONE:
			buzzer_play_melody(long_tone, (sizeof(long_tone)/sizeof(note_t)));
			break;
		case POINT_TONE:
			buzzer_play_melody(short_victory_melody, (sizeof(short_victory_melody)/sizeof(note_t)));
			break;
		case VICTORY_TONE:
			buzzer_play_melody(long_victory_melody, (sizeof(long_victory_melody)/sizeof(note_t)));
			break;
		case LOST_TONE:
			buzzer_play_melody(lost_game_melody, (sizeof(lost_game_melody)/sizeof(note_t)));
			break;
		case LIVE_TONE:
			buzzer_play_melody(lost_life_melody, (sizeof(lost_life_melody)/sizeof(note_t)));
			break;
		}
	}
}

void buzzer_play_tone(uint16_t frequency, uint16_t duration) {
	buzzer_set_tone(frequency);
    vTaskDelay(duration);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void buzzer_set_tone(uint16_t frequency) {
    if (frequency == 0) {
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    } else {
        __HAL_TIM_SET_AUTORELOAD(&htim3, 1000000 / frequency);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 50);
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    }
}

void buzzer_play_melody(note_t *melody, int num_notes) {
    for (int i = 0; i < num_notes; i++) {
    	buzzer_play_tone(melody[i].frequency, melody[i].duration);
    }
}
