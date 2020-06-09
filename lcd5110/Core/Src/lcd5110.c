/*
 * lcd5110.c
 *
 *  Created on: Jun 9, 2020
 *      Author: winxo
 */
#include <stm32f1xx_hal.h>
#include <main.h>
#include "font.h"
#include "lcd5110.h"

extern SPI_HandleTypeDef hspi1;
#define LCD_R 6
#define LCD_C 84
#define LCD_BUF_SZ (LCD_R*LCD_C)
static uint8_t _buf[LCD_BUF_SZ] = { 0 };

void write_byte(uint8_t data)
{
	HAL_SPI_Transmit(&hspi1, &data, 1, 0xff);
}
void write_cmd(uint8_t data)
{
	HAL_GPIO_WritePin(lcd_ce_GPIO_Port, lcd_ce_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(lcd_dc_GPIO_Port, lcd_dc_Pin, GPIO_PIN_RESET);
	write_byte(data);
	HAL_GPIO_WritePin(lcd_ce_GPIO_Port, lcd_ce_Pin, GPIO_PIN_SET);
}
void write_data(uint8_t data)
{
	HAL_GPIO_WritePin(lcd_ce_GPIO_Port, lcd_ce_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(lcd_dc_GPIO_Port, lcd_dc_Pin, GPIO_PIN_SET);
	write_byte(data);
	HAL_GPIO_WritePin(lcd_ce_GPIO_Port, lcd_ce_Pin, GPIO_PIN_SET);
}
void lcd5110_reset()
{
	HAL_GPIO_WritePin(lcd_rst_GPIO_Port, lcd_rst_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcd_rst_GPIO_Port, lcd_rst_Pin, GPIO_PIN_SET);
}
void lcd5110_refresh()
{
	write_cmd(0x40);
	write_cmd(0x80);
	HAL_GPIO_WritePin(lcd_ce_GPIO_Port, lcd_ce_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(lcd_dc_GPIO_Port, lcd_dc_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi1, _buf, LCD_BUF_SZ, 0xffff);
	HAL_GPIO_WritePin(lcd_ce_GPIO_Port, lcd_ce_Pin, GPIO_PIN_SET);
}
void lcd5110_clear()
{
	memset(_buf,0,LCD_BUF_SZ);
}
void lcd5110_pixel(uint8_t x, uint8_t y, uint8_t mode)
{
	if (x < 0 || x > 83 || y < 0 || y > 47)
	{
		return;
	}
	if(mode==LCD5110_PIXEL_MODE_PAINT)
	{
		_buf[(y >> 3) * 84 + x] |= 1 << (y & 0x07);
	}
	else
	{
		_buf[(y >> 3) * 84 + x] &=~( 1 << (y & 0x07));
	}

}
void lcd5110_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	for (int i = -w / 2; i < w / 2; i++)
	{
		for (int j = -h / 2; j < h / 2; j++)
		{
			lcd5110_pixel(x + i, y + j,LCD5110_PIXEL_MODE_PAINT);
		}
	}
}
void lcd5110_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	for (int i = -w / 2; i < w / 2; i++)
	{
		lcd5110_pixel(x + i, y - h / 2,LCD5110_PIXEL_MODE_PAINT);
		lcd5110_pixel(x + i, y + h / 2,LCD5110_PIXEL_MODE_PAINT);
	}
	for (int j = -h / 2; j < h / 2; j++)
	{
		lcd5110_pixel(x - w / 2, y + j,LCD5110_PIXEL_MODE_PAINT);
		lcd5110_pixel(x + w / 2, y + j,LCD5110_PIXEL_MODE_PAINT);
	}
}
void lcd5110_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	int tmp;
	int x, y;
	int dx, dy;
	int err;
	int ystep;
	uint8_t swapxy = 0;

	if (x1 > x2) dx = x1 - x2; else dx = x2 - x1;
	if (y1 > y2) dy = y1 - y2; else dy = y2 - y1;

	if (dy > dx)
	{
		swapxy = 1;
		tmp = dx; dx = dy; dy = tmp;
		tmp = x1; x1 = y1; y1 = tmp;
		tmp = x2; x2 = y2; y2 = tmp;
	}
	if (x1 > x2)
	{
		tmp = x1; x1 = x2; x2 = tmp;
		tmp = y1; y1 = y2; y2 = tmp;
	}
	err = dx >> 1;
	if (y2 > y1) ystep = 1; else ystep = -1;
	y = y1;
	for (x = x1; x <= x2; x++)
	{
		if (swapxy == 0)
		{
			lcd5110_pixel(x, y,LCD5110_PIXEL_MODE_PAINT);
		}
		else
		{
			lcd5110_pixel(y, x,LCD5110_PIXEL_MODE_PAINT);
		}
		err -= (uint8_t)dy;
		if (err < 0)
		{
			y += (int)ystep;
			err += (int)dx;
		}
	}
}
void lcd_putchar(char c, uint8_t x, uint8_t y)
{
	if (x < 0 || y < 0 || x >= 84 || y >= 6)
	{
		return;
	}
	const uint8_t *p = &font5x8[(c - ' ') * 5];
	for (int i = x; (i < x + 5) && i < 84; i++)
	{
		_buf[y * 84 + i] = *p;
		p++;
	}
}
void lcd5110_str(char* str, uint8_t x, uint8_t y)
{
	int offset = 0;
	while (*str)
	{
		lcd_putchar(*str, x + offset, y);
		offset += 6;
		str++;
	}
}
void lcd5110_init()
{
	lcd5110_reset();
	write_cmd(0x21);
	write_cmd(0x14);	/*bias*/
	write_cmd(0xba);	/*Vop*/
	write_cmd(0x20);
	write_cmd(0x0c);
	lcd5110_refresh();
}
