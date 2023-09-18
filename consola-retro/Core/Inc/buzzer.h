/*
 * buzzer.h
 *
 *  Created on: 18 sep. 2023
 *      Author: hernan
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <stdint.h>

typedef struct {
    uint16_t frequency;
    uint16_t duration;
} note_t;

void buzzer_test_melody(void);

void buzzer_play_tone(uint16_t frequency, uint16_t duration);

void buzzer_set_tone(uint16_t frequency);

void buzzer_play_melody(note_t *melody, int num_notes);

#endif /* INC_BUZZER_H_ */
