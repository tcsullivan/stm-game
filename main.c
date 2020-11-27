/**
 * main.c - Firmware entry point and main loop for game or testing.
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
#include "dogs.h"
#include "hal.h"

/*
 * Progress:
 *  - Serial through LPUART1 works (38400 baud, takes over swdio pins)
 *  - Display comm. over SPI, can clear screen
 *  - Can read buttons through PAL (through interrupts now)
 */

THD_WORKING_AREA(waThread2, 96);
THD_FUNCTION(Thread2, arg)
{
    (void)arg;

    dogs_init();

    while (1) {
        chThdSleepMilliseconds(100);
        dogs_buffer[0] = button_state;
        dogs_buffer[1] = button_state;
        dogs_buffer[2] = button_state;
        dogs_buffer[3] = button_state;
        dogs_flush();
    }
}

THD_TABLE_BEGIN
    THD_TABLE_THREAD(0, "game", waThread2, Thread2, NULL)
THD_TABLE_END

int main(void)
{
    halInit();
    chSysInit();

    buttons_init();

    // Below code for serial -- note that it cuts off debugging, and MUST be used in a thread
    //chThdSleepMilliseconds(2000);
    //palSetPadMode(GPIOA, 13, PAL_MODE_ALTERNATE(6));
    //palSetPadMode(GPIOA, 14, PAL_MODE_ALTERNATE(6));
    //sdStart(&LPSD1, NULL);
    //chnWrite(&LPSD1, (const uint8_t *)"Hello World!\r\n", 14);



    /* This is now the idle thread loop, you may perform here a low priority
       task but YOU MUST NEVER TRY TO SLEEP OR WAIT in this loop. Note that
       this tasks runs at the lowest priority level so any instruction added
       here will be executed after all other tasks have been started. */
    while (1);
}

