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

#include "bit-font2.h"
//#include "ch.h"
#include "dogs.h"
#include "hal.h"

#define SET_DATA palSetPad(GPIOC, 15)
#define SET_CMD  palClearPad(GPIOC, 15)
#define CS_HIGH  palSetPad(GPIOA, 4)
#define CS_LOW   palClearPad(GPIOA, 4)
unsigned char dogs_buffer[DISP_WIDTH * DISP_HEIGHT / 8];

//static volatile bool dogs_spi_done = false;

#define spi_send(data, len) spiSend(&SPID1, len, data)
//static void spi_send(unsigned char *data, unsigned int len)
//{
    //dogs_spi_done = false;
    //spiStartSend(&SPID1, len, data);
    //while (!dogs_spi_done)
    //    asm("wfi");

    //for (; len > 0; --len)
    //    spiPolledExchange(&SPID1, *data++);
//}

static void dogs_init_display();
//static void dogs_spi_callback(SPIDriver *spid)
//{
//    if (spiIsBufferComplete(spid))
//        dogs_spi_done = true;
//}

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

    static const SPIConfig spicfg = {
        false,
        NULL /*dogs_spi_callback*/,
        0, // cr1
        0
    };
    spiStart(&SPID1, &spicfg);

    dogs_init_display();
    dogs_clear();
    dogs_flush();
}

void dogs_write_cmd(unsigned char byte)
{
    spi_send(&byte, 1);
}

void dogs_set_column(unsigned int col)
{
    dogs_write_cmd(0x10 | ((col >> 4) & 0xF));
    dogs_write_cmd(0x00 | (col & 0xF));
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
    SET_CMD;
    CS_LOW;
    dogs_reset();
    CS_HIGH;

    unsigned long int reset_sleep = (STM32_SYSCLK / 1000) * 100;
    while (reset_sleep != 0) {
        asm("nop; nop; nop; nop; nop");
        reset_sleep -= 8;
    }

    CS_LOW;
    dogs_set_scroll_line(0);
    dogs_set_segdir(true);
    dogs_set_comdir(false);
    dogs_set_pixelson(false);
    dogs_set_invert(false);
    dogs_set_bias(true);
    dogs_set_power(0x07);
    dogs_set_vlcd_ratio(7);
    dogs_set_contrast(12);
    dogs_set_advanced(0x83);
    dogs_set_sleep(false);
    CS_HIGH;
}

void dogs_clear()
{
    uint32_t *ptr = (uint32_t *)dogs_buffer;
    unsigned int count = sizeof(dogs_buffer) / sizeof(uint32_t) / 12;

    for (; count; --count) {
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
    }
}

void dogs_flush()
{
    unsigned char *ptr = dogs_buffer;
    CS_LOW;
    for (int page = 0; page < 8; ++page) {
        SET_CMD;
        dogs_set_page(page);
        dogs_set_column(30);

        SET_DATA;
        spi_send(ptr, 102);
        ptr += 102;
    }
    CS_HIGH;
}

void draw_pixel(int x, int y, bool state)
{
    if (x < 0 || y < 0 || x >= DISP_WIDTH || y >= DISP_HEIGHT)
        return;
    y = DISP_HEIGHT - 1 - y;
    if (state)
        dogs_buffer[y / 8 * DISP_WIDTH + x] |= (1 << (y % 8));
    else
        dogs_buffer[y / 8 * DISP_WIDTH + x] &= ~(1 << (y % 8));
}

void draw_rect(int x, int y, int w, int h)
{
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++)
            draw_pixel(x + i, y + j, true);
    }
}

void draw_bitmap_wh(int x, int y, int w, int h, const unsigned char *data)
{
    int sbit = 0;

    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            draw_pixel(x + i, y + j, *data & (1 << sbit));
            if (++sbit == 8) {
                sbit = 0;
                ++data;
            }
        }
    }
}

void draw_bitmap(int x, int y, const unsigned char *buffer)
{
    draw_bitmap_wh(x, y, buffer[0], buffer[1], buffer + 2);
}

void draw_glyph(int ix, int iy, unsigned int glyph)
{
    const unsigned char *ptr;
    int x, y, sbit, tbits;

    x = ix + 5;
    y = iy;
    ptr = bitfontGetBitmapLo(glyph);
    sbit = 0;
    tbits = 18;

draw_glyph_loop:
    for (int i = 0; i < tbits; ++i) {
        draw_pixel(x, y, *ptr & (1 << sbit));

        if (x == ix) {
            x += 5;
            ++y;
        } else {
            --x;
        }

        if (sbit == 7) {
            sbit = 0;
            ++ptr;
        } else {
            ++sbit;
        }
    }

    if (tbits == 18) {
        ptr = bitfontGetBitmapHi(glyph);
        sbit = 0;
        tbits = 24;
        goto draw_glyph_loop;
    }
}

void draw_number(int x, int y, int number)
{
    // TODO Handle negatives better?
    if (number < 0)
        number = -number;

    // Count digits
    int tmp = number;
    int count;
    for (count = 0; tmp; count++)
        tmp /= 10;
    if (count == 0)
        count = 1;

    // Draw digits
    x += count * 7;
    do {
        x -= 7;
        draw_glyph(x, y, bitfontGetGlyph('0' + (number % 10)));
    } while (number /= 10);
}

