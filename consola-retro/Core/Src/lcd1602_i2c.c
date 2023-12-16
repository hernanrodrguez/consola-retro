/*
 * lcd1602_i2c.c
 *
 *  Created on: Mar 29, 2023
 *      Author: carlassaraf
 */

#include "lcd1602_i2c.h"

// I2C handler
#ifdef STM32F103xB
I2C_HandleTypeDef *i2c;
#elif defined(__LPC17XX__)
I2C_ID_T i2c;
#endif

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val) {
	// Envio byte por I2C
	i2c_transmit_byte(i2c, ADDR, val);
}

void lcd_toggle_enable(uint8_t val) {
    // Toggle enable pin on LCD display
	for(uint32_t i = 0; i < 5000; i++);
	i2c_write_byte(val | LCD_ENABLE_BIT);
	for(uint32_t i = 0; i < 5000; i++);
	i2c_write_byte((uint8_t)(val & ~LCD_ENABLE_BIT));
	for(uint32_t i = 0; i < 5000; i++);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(int line, int position) {

	int val;
	switch(line) {
		case 0:
			val = 0x80 + position;  // Dirección base de la primera línea
			break;
		case 1:
			val = 0xC0 + position;  // Dirección base de la segunda línea
			break;
		case 2:
			val = 0x94 + position;  // Dirección base de la tercera línea
			break;
		case 3:
			val = 0xD4 + position;  // Dirección base de la cuarta línea
			break;
		default:
			val = 0x80 + position;  // Por defecto, utilizar primera línea
			break;
	}
	lcd_send_byte(val, LCD_COMMAND);
}

uint8_t lcd_progressive_clear(uint8_t mode){
	return (lcd_progressive_print("                    ",
								  "                    ",
								  "                    ",
								  "                    ",
								  mode));
}

uint8_t lcd_progressive_print(const char *line_0, const char *line_1, const char *line_2, const char *line_3, uint8_t mode){

	static uint8_t i=0, j=0;
	static uint32_t start_ticks;
	static state_print_t state_print = PRINT;

	switch(state_print){

	case WAIT:
		if(HAL_GetTick() - start_ticks > PRINT_DELAY_LINES){
			state_print = PRINT;
		}
		break;

	case PRINT:

		if(mode){
			lcd_set_cursor(0, j);
			lcd_char(*(line_0+j));

			lcd_set_cursor(1, j);
			lcd_char(*(line_1+j));

			lcd_set_cursor(2, j);
			lcd_char(*(line_2+j));

			lcd_set_cursor(3, j);
			lcd_char(*(line_3+j));

			j++;
			if(j>19){
				j=0;
				return 1;
			}
		} else {
			lcd_set_cursor(i, j);

			switch(i){
			case 0:
				lcd_char(*(line_0+j));
				break;
			case 1:
				lcd_char(*(line_1+j));
				break;
			case 2:
				lcd_char(*(line_2+j));
				break;
			case 3:
				lcd_char(*(line_3+j));
				break;
			default:
				j=0;
				i=0;
				break;
			}

			j++;
			if(j>19){
				j=0;
				i++;
				if(i>3){
					i=0;
					return 1;
				}
			}
		}
		start_ticks = HAL_GetTick();
		state_print = WAIT;
		break;
	}

	return 0;
}

void lcd_print_ring(void){
	static uint8_t i=0, j=1, flag=1;
	static uint32_t start_ticks;
	static state_print_t state_print = PRINT;

	switch(state_print){

	case WAIT:
		if(HAL_GetTick() - start_ticks > PRINT_DELAY_RING){
			state_print = PRINT;
		}
		break;

	case PRINT:
		lcd_set_cursor(i, j);
		if(flag){
			lcd_char('*');
		} else {
			lcd_char(' ');
		}

		if(i==0){
			if(j==0){
				flag = !flag;
			}
			if(j>=0 && j<19){
				j++;
			} else if(j==19){
				i++;
			}
		} else if(i<3 && i>0){
			if(j==0){
				i--;
			} else if(j==19){
				i++;
			}
		} else if(i==3){
			if(j>0 && j<=19){
				j--;
			} else if(j==0){
				i--;
			}
		}
		start_ticks = HAL_GetTick();
		state_print = WAIT;
		break;
	}
}

void lcd_horizontal_print(const char *text, uint8_t line){
	static uint8_t i=0, len=0;
	static const char *previous_text = NULL;

	if(previous_text != text){
		previous_text = text;
		i=0;
		len = str_len(text);
	}

	if((len-i)>20){ // tengo que seguir avanzando
		lcd_set_cursor(line, 0);
		lcd_print_line(text+i);
		i++;
	}
}

uint32_t str_len(const char *str){
	uint32_t i=0;
    while (*str) {
    	*str++;
        i++;
    }
    return i;
}

void lcd_print_time(uint8_t minutes, uint8_t seconds){
	char print_num;
	for(uint8_t j=8; j<13; j++){
		if(j==8){
			print_num = ((minutes/10)%10)+'0';
		} else if(j==9){
			print_num = (minutes%10)+'0';
		} else if(j==10){
			print_num = ':';
		} else if(j==11){
			print_num = ((seconds/10)%10)+'0';
		} else if(j==12){
			print_num = (seconds%10)+'0';
		}
		lcd_set_cursor(3, j);
		lcd_char(print_num);
	}
}

void lcd_print_score(uint8_t score_1, uint8_t score_2){
	char print_num;

	print_num = ((score_1/10)%10)+'0';
	lcd_set_cursor(2, 6);
	lcd_char(print_num);

	print_num = (score_1%10)+'0';
	lcd_set_cursor(2, 7);
	lcd_char(print_num);

	print_num = ((score_2/10)%10)+'0';
	lcd_set_cursor(2, 16);
	lcd_char(print_num);

	print_num = (score_2%10)+'0';
	lcd_set_cursor(2, 17);
	lcd_char(print_num);
}

void lcd_print_tetris_score(uint32_t score, uint8_t pos){
	char print_num;
	uint8_t x, y;

	switch(pos){
	case 0:
		x=4;
		y=0;
		break;
	case 1:
		x=12;
		y=0;
		break;
	case 2:
		x=4;
		y=1;
		break;
	case 3:
		x=12;
		y=1;
		break;
	case 4:
		x=4;
		y=2;
		break;
	case 5:
		x=12;
		y=2;
		break;
	}

	print_num = ((score/100)%10)+'0';
	lcd_set_cursor(y, x);
	lcd_char(print_num);

	print_num = ((score/10)%10)+'0';
	lcd_set_cursor(y, x+1);
	lcd_char(print_num);

	print_num = (score%10)+'0';
	lcd_set_cursor(y, x+2);
	lcd_char(print_num);

	lcd_set_cursor(y, x+3);
	lcd_char('0');
}

void lcd_print_snake_score(uint32_t score, uint8_t pos){
	char print_num;
	uint8_t x, y;

	switch(pos){
	case 0:
		x=3;
		y=0;
		break;
	case 1:
		x=9;
		y=0;
		break;
	case 2:
		x=15;
		y=0;
		break;
	case 3:
		x=3;
		y=1;
		break;
	case 4:
		x=9;
		y=1;
		break;
	case 5:
		x=15;
		y=1;
		break;
	case 6:
		x=3;
		y=2;
		break;
	case 7:
		x=9;
		y=2;
		break;
	case 8:
		x=15;
		y=2;
		break;
	}

	print_num = ((score/10)%10)+'0';
	lcd_set_cursor(y, x);
	lcd_char(print_num);

	print_num = (score%10)+'0';
	lcd_set_cursor(y, x+1);
	lcd_char(print_num);

}

void lcd_print_pong_score(uint8_t score_1, uint8_t score_2, uint8_t pos) {
	char print_num;
	uint8_t x, y;

	switch(pos){
	case 0:
		x=5;
		y=0;
		break;
	case 1:
		x=0;
		y=1;
		break;
	case 2:
		x=10;
		y=1;
		break;
	case 3:
		x=0;
		y=2;
		break;
	case 4:
		x=10;
		y=2;
		break;
	}

	print_num = ((score_1/10)%10)+'0';
	lcd_set_cursor(y, x+2);
	lcd_char(print_num);

	print_num = (score_1%10)+'0';
	lcd_set_cursor(y, x+3);
	lcd_char(print_num);

	lcd_set_cursor(y, x+4);
	lcd_char('-');

	print_num = ((score_2/10)%10)+'0';
	lcd_set_cursor(y, x+5);
	lcd_char(print_num);

	print_num = (score_2%10)+'0';
	lcd_set_cursor(y, x+6);
	lcd_char(print_num);
}

void lcd_print_pts_lives(uint32_t score, uint8_t lives){
	char print_num;

	print_num = ((score/10)%10)+'0';
	lcd_set_cursor(2, 7);
	lcd_char(print_num);

	print_num = (score%10)+'0';
	lcd_set_cursor(2, 8);
	lcd_char(print_num);

	print_num = lives + '0';
	lcd_set_cursor(2, 17);
	lcd_char(print_num);
}

void lcd_print_pts(uint32_t score){
	char print_num;

	print_num = ((score/1000)%10)+'0';
	lcd_set_cursor(2, 11);
	lcd_char(print_num);

	print_num = ((score/100)%10)+'0';
	lcd_set_cursor(2, 12);
	lcd_char(print_num);

	print_num = ((score/10)%10)+'0';
	lcd_set_cursor(2, 13);
	lcd_char(print_num);

	print_num = (score%10)+'0';
	lcd_set_cursor(2, 14);
	lcd_char(print_num);
}

void lcd_print(const char *line0, const char *line1, const char *line2, const char *line3) {
	lcd_set_cursor(0, 0);
	for(uint8_t i=0; i<20; i++) {
        lcd_char(*line0++);
    }
	lcd_set_cursor(1, 0);
	for(uint8_t i=0; i<20; i++) {
        lcd_char(*line1++);
    }
	lcd_set_cursor(2, 0);
	for(uint8_t i=0; i<20; i++) {
        lcd_char(*line2++);
    }
	lcd_set_cursor(3, 0);
	for(uint8_t i=0; i<20; i++) {
        lcd_char(*line3++);
    }
}

void lcd_print_line(const char *s) {
    for(uint8_t i=0; i<20; i++) {
        lcd_char(*s++);
    }
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

#ifdef STM32F103xB
void lcd_init(I2C_HandleTypeDef *hi2c1) {
	// Guardo el I2C usado
	i2c = hi2c1;
#elif defined(__LPC17XX__)
void lcd_init(I2C_ID_T i2c_id) {
	// Guardo el I2C usado
	i2c = i2c_id;
#endif

    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}
