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

note_t marioBrosMelody[] = {
    {659, 250},
    {0, 100}, // Pausa
    {659, 250},
    {0, 100}, // Pausa
    {659, 250},
    {0, 100}, // Pausa
    {523, 250},
    {659, 250},
    {0, 100}, // Pausa
    {784, 250},
    {0, 400}, // Pausa
    {392, 250},
    {0, 400}, // Pausa
    {523, 250},
    {0, 400}, // Pausa
    {392, 250},
};

note_t twinkleTwinkleMelody[] = {
    {523, 400},
    {523, 400},
    {659, 400},
    {659, 400},
    {783, 400},
    {783, 400},
    {659, 800},
    {523, 400},
    {523, 400},
    {392, 400},
    {392, 400},
    {329, 400},
    {329, 400},
    {261, 800},
};

note_t jingleBellsMelody[] = {
    {659, 250},
    {659, 250},
    {0, 250}, // Pausa
    {659, 250},
    {659, 250},
    {0, 250}, // Pausa
    {659, 250},
    {0, 250}, // Pausa
    {659, 250},
    {659, 250},
    {659, 250},
    {659, 250},
    {587, 250},
    {587, 250},
    {659, 250},
    {0, 250}, // Pausa
};

note_t happyBirthdayMelody[] = {
    {392, 400},
    {392, 400},
    {440, 800},
    {392, 400},
    {523, 800},
    {392, 400},
    {392, 400},
    {440, 800},
    {392, 400},
    {659, 800},
    {587, 400},
    {523, 400},
    {349, 800},
};

note_t harryPotterMelody[] = {
    {392, 400},
    {523, 400},
    {587, 400},
    {739, 600},
    {659, 200},
    {587, 400},
    {659, 600},
    {783, 600},
    {987, 600},
    {880, 400},
    {783, 400},
    {880, 400},
    {1046, 800},
    {987, 400},
    {880, 400},
    {783, 400},
    {659, 600},
    {587, 200},
    {659, 400},
    {987, 400},
    {880, 400},
    {783, 400},
    {659, 600},
    {587, 200},
    {523, 400},
};

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

note_t powerOnMelody2[] = {
    {523, 200},
    {659, 200},
    {784, 200},
    {880, 400},
    {784, 200},
    {880, 400},
    {1046, 400},
    {1175, 400},
};

note_t powerOnMelody3[] = {
    {587, 300},
    {698, 300},
    {784, 300},
    {880, 600},
    {784, 300},
    {880, 600},
    {1046, 600},
    {1175, 600},
};

note_t inGameMelody1[] = {
    {440, 300},
    {523, 300},
    {587, 300},
    {659, 600},
    {587, 300},
    {659, 600},
};

note_t inGameMelody2[] = {
    {523, 200},
    {659, 200},
    {784, 200},
    {880, 400},
    {784, 200},
    {880, 400},
};

note_t inGameMelody3[] = {
    {659, 300},
    {784, 300},
    {880, 300},
    {987, 600},
    {880, 300},
    {987, 600},
};

note_t inGameMelody4[] = {
    {523, 200},
    {659, 200},
    {784, 200},
    {880, 400},
    {784, 200},
    {880, 400},
};

note_t inGameMelody5[] = {
    {523, 300},
    {659, 300},
    {784, 300},
    {880, 600},
};

note_t inGameMelody6[] = {
    {523, 400},
    {659, 400},
    {784, 400},
    {880, 800},
};

note_t short_victory_melody[] = {
    {784, 400},
    {987, 400},
    {1175, 400},
    {1397, 800},
};

note_t victoryMelody2[] = {
    {523, 300},
    {659, 300},
    {784, 300},
    {880, 600},
};

note_t victoryMelody3[] = {
    {587, 300},
    {698, 300},
    {784, 300},
    {880, 600},
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

note_t victoryMelody5[] = {
    {659, 100},
    {784, 100},
    {988, 100},
    {1174, 200},
    {1318, 200},
    {1760, 100},
    {1976, 100},
    {2093, 200},
    {2637, 200},
    {3136, 400},
};

note_t victoryMelody6[] = {
	{523, 100},
	{622, 100},
	{784, 100},
	{880, 200},
	{1046, 200},
	{1174, 100},
	{1568, 100},
	{1760, 200},
	{2093, 200},
	{2637, 400},
};

note_t lost_life_melody[] = {
    {392, 200},
    {294, 200},
    {262, 200},
    {196, 400},
};

note_t defeatMelody2[] = {
    {523, 300},
    {392, 300},
    {294, 300},
    {233, 600},
};

note_t defeatMelody3[] = {
    {349, 300},
    {294, 300},
    {233, 300},
    {196, 600},
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

void buzzer_test_melody(void) {
	buzzer_play_melody(victoryMelody6, (sizeof(victoryMelody6)/sizeof(note_t)));
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
