#include "stm32f1xx_hal.h"
#include <stdint.h>

static SPI_HandleTypeDef *spi_handler;
static GPIO_TypeDef *gpio_cs;
static uint16_t pin_cs;

static void send_cmd(uint32_t address, uint32_t data){
	uint16_t serial_data;
	serial_data = ((address & 0x0F) << 8) | (data & 0xFF);
	HAL_GPIO_WritePin(gpio_cs, pin_cs, GPIO_PIN_RESET);
	HAL_SPI_Transmit(spi_handler, (void*) &serial_data, 1, 10);
	HAL_GPIO_WritePin(gpio_cs, pin_cs, GPIO_PIN_SET);
}

void max7219_clear(){
	for(uint8_t i=0; i<8; i++)
		send_cmd(i+1, 0x00);
}

void max7219_print(uint8_t *data){
	for(uint8_t i=0; i<8; i++)
		send_cmd(i+1, data[i]);
}

void max7219_init(
		SPI_HandleTypeDef *spi,
		GPIO_TypeDef *GPIO_cs,
		uint16_t GPIO_Pin_cs){
	spi_handler = spi;
	gpio_cs = GPIO_cs;
	pin_cs = GPIO_Pin_cs;

	send_cmd(0x0C, 0x01);	//Enciendo
	send_cmd(0x0F, 0x00);	//Salgo de modoTest
	send_cmd(0x09, 0x00);	//Sin decodificar
	send_cmd(0x0B, 0x07);	//Muestro todas las lineas.
	send_cmd(0x0A, 0x0F);	//Intensidad

	max7219_clear();	//Borro la pantalla.

}
