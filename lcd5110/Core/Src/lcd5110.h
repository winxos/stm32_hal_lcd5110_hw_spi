/*
 * lcd5110.h
 *
 *  Created on: Jun 9, 2020
 *      Author: winxo
 */

#ifndef SRC_LCD5110_H_
#define SRC_LCD5110_H_

#define LCD5110_PIXEL_MODE_PAINT 	0
#define LCD5110_PIXEL_MODE_CLEAR	1

void lcd5110_init();
void lcd5110_refresh();
void lcd5110_pixel(uint8_t x, uint8_t y, uint8_t mode);
void lcd5110_str(char* str, uint8_t x, uint8_t y);
void lcd5110_clear();

void lcd5110_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void lcd5110_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif /* SRC_LCD5110_H_ */
