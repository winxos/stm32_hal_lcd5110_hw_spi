/*
 * setup.c
 *
 *  Created on: Jun 9, 2020
 *      Author: winxo
 */
#include <stm32f1xx_hal.h>
#include "lcd5110.h"
#include <math.h>
int __io_putchar(int ch)
{
	ITM_SendChar(ch);
	return 0;
}
void lcd5110_init();
void draw(uint64_t x)
{
	lcd5110_box(sin(x/20.0)*30+42, 24, 10, 10);
	lcd5110_box(cos(x/20.0)*30+42, sin(x/10.0)*14+24, 10, 10);
}
void setup()
{
	lcd5110_init();
	uint64_t x=0;
	for(;;)
	{
		lcd5110_clear();
		draw(x);
		x++;
		lcd5110_refresh();
		HAL_Delay(10); /**/
	}
}
