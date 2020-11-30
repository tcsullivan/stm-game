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
 *  - Use ADC to read Vintref, print to screen in mV
 *  - Sleep mode via WFI, saves ~0.5mA (we're running around 1.1mA)
 *  - Run at 512kHz, only use HSI for ADC: 360uA (jumpy)
 *  - Drop to 1.2V Vcore (range 3), enable low-V detector: 375uA (steady) (440uA at 1MHz)
 *  - Run at 4MHz, drop to low-power run/sleep @ 64kHz for idle: 375uA (also lowered contrast)
 */

static volatile bool adc_is_complete = false;
static void adc_callback(ADCDriver *adcd)
{
    (void)adcd;
    adc_is_complete = true;
}

static const ADCConfig adccfg = {
    .dummy = 0
};

static const ADCConversionGroup adcgrpcfg = {
  .circular     = false,
  .num_channels = 1,
  .end_cb       = adc_callback,
  .error_cb     = NULL,
  .cfgr1        = ADC_CFGR1_RES_12BIT,     /* CFGR1 */
  .cfgr2        = 0,                       /* CFGR2 */
  .tr           = ADC_TR(0, 0),            /* TR */
  .smpr         = ADC_SMPR_SMP_1P5,        /* SMPR */
  .chselr       = ADC_CHSELR_CHSEL17       /* CHSELR */
};

static int readVddmv()
{
    adcsample_t reading = 0;

    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    adcStart(&ADCD1, &adccfg);
    adcSTM32EnableVREF(&ADCD1);
    adcStartConversion(&ADCD1, &adcgrpcfg, &reading, 1);
    while (!adc_is_complete);
    adcStopConversion(&ADCD1);
    adcSTM32DisableVREF(&ADCD1);
    adcStop(&ADCD1);

    RCC->CR &= ~RCC_CR_HSION;

    return 3000 * /* CAL */ *((adcsample_t *)0x1FF80078) / reading;
}

THD_WORKING_AREA(waThread2, 128);
THD_FUNCTION(Thread2, arg)
{
    (void)arg;

    dogs_init();

    const unsigned char testbitmap[] = {
        8, 8,
        0b00111100,
        0b01100110,
        0b01000010,
        0b01111110,
        0b01100110,
        0b01000010,
        0b11000000,
        0b11000000,
    };

    int t1x = DISP_WIDTH, t1o = 30;

    int py = DISP_HEIGHT / 2 - 4;
    int vy = 0;
    int counter = 0;
    int mv = readVddmv();
    while (1) {
        //systime_t old_time = chVTGetSystemTimeX();

        if (py > 0) {
            py += vy;
            if (vy > -4)
                vy--;
        } else if (py < 0) {
            py = 0;
        }

        if (button_state & BUTTON_2) {
            vy = 5;
            if (py <= 0)
                py = 1;
        }

        dogs_clear();

        draw_rect(t1x, 0, 4, t1o - 12);
        draw_rect(t1x, t1o + 12, 4, DISP_HEIGHT - t1o + 12);

        draw_bitmap(4, py, testbitmap);

        draw_number(0, 50, mv);
        dogs_flush();



        if (++counter == 50) {
            counter = 0;
            mv = !(PWR->CSR & PWR_CSR_PVDO) ? readVddmv() : 1;
        }
        t1x -= 2;
        if (t1x <= -5)
            t1x = DISP_WIDTH;

        //chThdSleepUntilS(chTimeAddX(old_time, TIME_MS2I(100) / 32));
        chThdSleepS(TIME_MS2I(100) / 64);
    }
}

THD_TABLE_BEGIN
    THD_TABLE_THREAD(0, "game", waThread2, Thread2, NULL)
THD_TABLE_END

int main(void)
{
    halInit();
    chSysInit();

    RCC->CR &= ~RCC_CR_HSION;
    PWR->CR |= PWR_CR_LPSDSR;

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
    while (1)
        asm("wfi");
}

void HardFault_Handler()
{
    while (1);
}

