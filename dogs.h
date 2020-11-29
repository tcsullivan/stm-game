/**
 * dogs.h - Interface for drawing to the EADOGS102N-6 display.
 * Copyright (C) 2020  Clyne Sullivan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * See the GNU General Public License for more details.
 */

#ifndef DOGS_H_
#define DOGS_H_

#define DISP_WIDTH  102
#define DISP_HEIGHT 64

extern unsigned char dogs_buffer[DISP_WIDTH * DISP_HEIGHT / 8];

void dogs_init();
void dogs_clear();
void dogs_flush();

void draw_pixel(int x, int y, bool state);
void draw_bitmap(int x, int y, const unsigned char *buffer);
void draw_number(int x, int y, int number);

#endif // DOGS_H_

