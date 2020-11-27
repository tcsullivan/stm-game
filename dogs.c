/**
 * dogs.c - Interface for drawing to the EADOGS102N-6 display.
 * Copyright (C) 2020  Clyne Sullivan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * See the GNU General Public License for more details.
 */

#include "ch.h"
#include "dogs.h"
#include "hal.h"

#define SET_DATA palSetPad(GPIOC, 15)
#define SET_CMD  palClearPad(GPIOC, 15)
#define CS_HIGH  palSetPad(GPIOA, 4)
#define CS_LOW   palClearPad(GPIOA, 4)

unsigned char dogs_buffer[DISP_WIDTH * DISP_HEIGHT / 8];

static void dogs_init_display();

void dogs_init()
{
    // SPI
    palSetPadMode(GPIOB, 1, PAL_MODE_ALTERNATE(1));     // MOSI
    palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(0));     // CLK
    palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);  // nCS
    palSetPadMode(GPIOC, 14, PAL_MODE_OUTPUT_PUSHPULL); // nRST
    palSetPadMode(GPIOC, 15, PAL_MODE_OUTPUT_PUSHPULL); // CD

    palSetPad(GPIOA, 4);
    palSetPad(GPIOC, 14);
    palClearPad(GPIOC, 15);

    static const SPIConfig ls_spicfg = {
        false,
        NULL,
        SPI_CR1_BR_2 | SPI_CR1_BR_1,
        0
    };
    spiStart(&SPID1, &ls_spicfg);

    dogs_init_display();
    dogs_clear();
}

void dogs_write_data(unsigned char byte)
{
    SET_DATA;
    spiSend(&SPID1, 1, &byte);
}
void dogs_write_cmd(unsigned char byte)
{
    SET_CMD;
    spiSend(&SPID1, 1, &byte);
}

void dogs_set_column(unsigned int col)
{
    //if (col < DISP_WIDTH) {
        dogs_write_cmd(0x10 | ((col >> 4) & 0xF));
        dogs_write_cmd(0x00 | (col & 0xF));
    //}
}
void dogs_set_power(unsigned int bits)
{
    dogs_write_cmd(0x28 | (bits & 0x7));
}
void dogs_set_scroll_line(unsigned int startline)
{
    dogs_write_cmd(0x40 | (startline & 0x3F));
}
void dogs_set_page(unsigned int page)
{
    dogs_write_cmd(0xB0 | (page & 0xF));
}
void dogs_set_vlcd_ratio(unsigned int ratio)
{
    dogs_write_cmd(0x20 | (ratio & 0x7));
}
void dogs_set_contrast(unsigned int value)
{
    dogs_write_cmd(0x81);
    dogs_write_cmd(0x00 | (value & 0x3F));
}
void dogs_set_pixelson(bool on)
{
    dogs_write_cmd(on ? 0xA5 : 0xA4);
}
void dogs_set_invert(bool invert)
{
    dogs_write_cmd(invert ? 0xA7 : 0xA6);
}
void dogs_set_sleep(bool sleep)
{
    dogs_write_cmd(sleep ? 0xAE : 0xAF);
}
void dogs_set_segdir(bool normal)
{
    dogs_write_cmd(normal ? 0xA0 : 0xA1);
}
void dogs_set_comdir(bool normal)
{
    dogs_write_cmd(normal ? 0xC0 : 0xC8);
}
void dogs_reset()
{
    dogs_write_cmd(0xE2);
}
void dogs_set_bias(bool higher)
{
    dogs_write_cmd(higher ? 0xA2 : 0xA3);
}
void dogs_set_advanced(unsigned int bits)
{
    dogs_write_cmd(0xFA);
    dogs_write_cmd(0x10 | bits);
}

void dogs_init_display()
{
    CS_LOW;
    dogs_reset();
    CS_HIGH;
    chThdSleepMilliseconds(100);
    CS_LOW;
    dogs_set_scroll_line(0);
    dogs_set_segdir(true);
    dogs_set_comdir(false);
    dogs_set_pixelson(false);
    dogs_set_invert(false);
    dogs_set_bias(true);
    dogs_set_power(0x07);
    dogs_set_vlcd_ratio(7);
    dogs_set_contrast(0x10);
    dogs_set_advanced(0x83);
    dogs_set_sleep(false);
    CS_HIGH;
}

void dogs_clear()
{
    unsigned char *ptr = dogs_buffer;
    for (unsigned int i = 0; i < sizeof(dogs_buffer); i++) {
        *ptr++ = 0;
    }
    dogs_flush();
}

void dogs_flush()
{
    unsigned char *ptr = dogs_buffer;
    CS_LOW;
    for (int page = 0; page < 8; page++) {
        dogs_set_page(page);
        dogs_set_column(30);
        for (int i = 0; i < 102; i++)
            dogs_write_data(*ptr++);
    }
    CS_HIGH;
}

