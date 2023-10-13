/*
 * buzzer.h
 *
 *  Created on: 18 sep. 2023
 *      Author: hernan
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <stdint.h>

#define OPENING_MELODY 	0
#define NAVIGATION_TONE 1
#define LONG_TONE 		2
#define POINT_TONE 		3
#define LIVE_TONE 		4
#define VICTORY_TONE	5
#define LOST_TONE		6

typedef struct {
    uint16_t frequency;
    uint16_t duration;
} note_t;

void buzzer_play(void);

void buzzer_test_melody(void);

void buzzer_play_tone(uint16_t frequency, uint16_t duration);

void buzzer_set_tone(uint16_t frequency);

void buzzer_play_melody(note_t *melody, int num_notes);

#endif /* INC_BUZZER_H_ */
