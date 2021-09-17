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
 *  - Use RTC for delay, Stop mode when idle: 348uA
 *
 *  - Flappy bird is going, 2048 next
 */

static int readVddmv();

THD_WORKING_AREA(waThread2, 128);
THD_FUNCTION(Thread2, arg)
{
    (void)arg;

    dogs_init();
    flapbird_init();
}
THD_TABLE_BEGIN
    THD_TABLE_THREAD(0, "game", waThread2, Thread2, NULL)
THD_TABLE_END

static void alarm_callback(RTCDriver *rtcp, rtcevent_t event)
{
    (void)rtcp;
    (void)event;

    static bool sleep = false;

	bool sleep_button = (button_state & BUTTON_1) != 0;
    if (sleep && !sleep_button)
		return;

    RCC->ICSCR |= 6 << RCC_ICSCR_MSIRANGE_Pos;
    dogs_set_sleep(false);

    if (sleep_button) {
        sleep ^= true;
        if (sleep) {
            draw_number(DISP_WIDTH - 33, 0,
                        !(PWR->CSR & PWR_CSR_PVDO) ? readVddmv() : 1);
            dogs_flush();
        }
    }

    if (!sleep)
        flapbird_loop();

    dogs_set_sleep(true);
}

int main(void)
{
    halInit();
    chSysInit();
    buttons_init();

    static const RTCWakeup wakeupcfg = {
        (0 << 16) | // wucksel (37k /16 = ~2k)
        240         // wut (hope for 10Hz)
    };
    rtcSTM32SetPeriodicWakeup(&RTCD1, &wakeupcfg);
    rtcSetCallback(&RTCD1, alarm_callback);

    RCC->CR &= ~RCC_CR_HSION;
    PWR->CR |= PWR_CR_LPSDSR | PWR_CR_ULP;
    PWR->CR |= PWR_CR_LPRUN;
    SCB->SCR = 6;
    FLASH->ACR |= FLASH_ACR_SLEEP_PD;

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
        asm("wfe");
}

void HardFault_Handler()
{
    while (1);
}

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

int readVddmv()
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
