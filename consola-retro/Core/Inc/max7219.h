/*
 * max7219.h
 *
 *  Created on: Jul 1, 2020
 *      Author: esala
 */

#ifndef INC_MAX7219_H_
#define INC_MAX7219_H_

void max7219_clear();
void max7219_print(uint8_t *data);
void max7219_init(
		SPI_HandleTypeDef *spi,
		GPIO_TypeDef *GPIO_cs,
		uint16_t GPIO_Pin_cs);


#endif /* INC_MAX7219_H_ */
