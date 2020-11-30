/**
 * buttons.c - Provides ability to read button state.
 * Copyright (C) 2020  Clyne Sullivan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * See the GNU General Public License for more details.
 */

#include "buttons.h"
#include "ch.h"
#include "hal.h"

unsigned char button_state = 0;

static void buttonStateHandler(void *arg)
{
    int pad = (unsigned int)arg & 0xFF;
    int bit = 1 << ((unsigned int)arg >> 8);

    if (!palReadPad(GPIOA, pad)) {
        if ((button_state & bit) != bit)
            button_state |= bit;
    } else {
        if ((button_state & bit) == bit)
            button_state &= ~(bit);
    }
}

void buttons_init()
{
    palSetPadMode(GPIOA, 0, PAL_MODE_INPUT_PULLUP);   // Joy A (UL)
    palSetPadMode(GPIOA, 1, PAL_MODE_INPUT_PULLUP);   // Joy B (UR)
    palSetPadMode(GPIOA, 2, PAL_MODE_INPUT_PULLUP);   // Joy C (DR)
    palSetPadMode(GPIOA, 3, PAL_MODE_INPUT_PULLUP);   // Joy D (DL)
    palSetPadMode(GPIOA, 6, PAL_MODE_INPUT_PULLUP);   // Joy button
    palSetPadMode(GPIOA, 7, PAL_MODE_INPUT_PULLUP);   // Button 1
    palSetPadMode(GPIOA, 9, PAL_MODE_INPUT_PULLUP);   // Button 2
    palSetPadMode(GPIOA, 10, PAL_MODE_INPUT_PULLUP);  // Button 3
    palEnablePadEvent(GPIOA, 0, PAL_EVENT_MODE_BOTH_EDGES);
    palEnablePadEvent(GPIOA, 1, PAL_EVENT_MODE_BOTH_EDGES);
    palEnablePadEvent(GPIOA, 2, PAL_EVENT_MODE_BOTH_EDGES);
    palEnablePadEvent(GPIOA, 3, PAL_EVENT_MODE_BOTH_EDGES);
    palEnablePadEvent(GPIOA, 6, PAL_EVENT_MODE_BOTH_EDGES);
    palEnablePadEvent(GPIOA, 7, PAL_EVENT_MODE_BOTH_EDGES);
    palEnablePadEvent(GPIOA, 9, PAL_EVENT_MODE_BOTH_EDGES);
    palEnablePadEvent(GPIOA, 10, PAL_EVENT_MODE_BOTH_EDGES);
    palSetPadCallback(GPIOA, 0, buttonStateHandler, (void *)0x0700);
    palSetPadCallback(GPIOA, 1, buttonStateHandler, (void *)0x0601);
    palSetPadCallback(GPIOA, 2, buttonStateHandler, (void *)0x0502);
    palSetPadCallback(GPIOA, 3, buttonStateHandler, (void *)0x0403);
    palSetPadCallback(GPIOA, 6, buttonStateHandler, (void *)0x0306);
    palSetPadCallback(GPIOA, 7, buttonStateHandler, (void *)0x0207);
    palSetPadCallback(GPIOA, 9, buttonStateHandler, (void *)0x0109);
    palSetPadCallback(GPIOA, 10, buttonStateHandler, (void *)0x000A);
}

