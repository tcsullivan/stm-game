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

#include "2048.h"
#include "flapbird.h"

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
 *  - Sleep display for 'pause': ~240uA
 *
 *  - Flappy bird is going, 2048 next
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

    flapbird_init();

    bool sleep = false;
    while (1) {
        if (button_state & BUTTON_1) {
            sleep ^= true;
            if (sleep) {
                draw_number(DISP_WIDTH - 33, 0,
                            !(PWR->CSR & PWR_CSR_PVDO) ? readVddmv() : 1);
                dogs_flush();
            }
            dogs_set_sleep(sleep);
        }

        int dtime = 100;
        if (!sleep) {
            dtime = flapbird_loop();
        }

        chThdSleepS(TIME_MS2I(dtime) / 64);
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

