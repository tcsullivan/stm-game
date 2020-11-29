/**
 * buttons.h - Provides ability to read button state.
 * Copyright (C) 2020  Clyne Sullivan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * See the GNU General Public License for more details.
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define BUTTON_JOYUL (1 << 7)
#define BUTTON_JOYUR (1 << 6)
#define BUTTON_JOYDR (1 << 5)
#define BUTTON_JOYDL (1 << 4)
#define BUTTON_JOY   (1 << 3)
#define BUTTON_1     (1 << 2)
#define BUTTON_2     (1 << 1)
#define BUTTON_3     (1 << 0)

extern unsigned char button_state;

void buttons_init();

#endif // BUTTONS_H_

