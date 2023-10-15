/*
 * conway.h
 *
 *  Created on: 14 oct. 2023
 *      Author: hernan
 */

#ifndef INC_CONWAY_H_
#define INC_CONWAY_H_

#include <stdint.h>

void conway_evolve(void);

void conway_init(void);

void conway_play(void);

void copy_matrix(uint32_t *dest, uint32_t *src);

uint8_t alive_neighbours(uint32_t *matrix, uint8_t i, uint8_t j);

void set_cell_state(uint32_t *matrix, uint8_t i, uint8_t j, uint8_t state);

uint8_t get_cell_state(uint32_t row, uint8_t col);

void conway_print(void);

//void init_matrix(uint8_t *matrix);

#endif /* INC_CONWAY_H_ */
