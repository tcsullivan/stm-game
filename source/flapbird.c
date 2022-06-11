#include "buttons.h"
#include "dogs.h"
#include "hal.h"

#include <stdint.h>

static const unsigned char bird[] = {
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

static int score;
static int t1x, t1o;
static int t2x, t2o;
static int py;
static int vy;
static int counter;

static uint32_t next = 31415926;

int rand()
{
    uint32_t tr = RTC->TR;
    uint32_t dr = RTC->DR;
    next = next * (0x197244 + dr) + tr;
    return (unsigned int)(next / 65536) % 32768;
}

#define NEW_BAR()	(rand() % 50 + 5)

void flapbird_init()
{
    score = 0;
    t1x = DISP_WIDTH / 2, t1o = NEW_BAR();
    t2x = DISP_WIDTH, t2o = NEW_BAR();
    py = DISP_HEIGHT / 2 - 4;
    vy = 0;
    counter = 0;
}

int flapbird_loop()
{
    // Player logic
    if (py > 0) {
        py += vy;
        if (vy > -3)
            vy--;
    } else {
        if (py < 0)
            py = 0;
        if (score > 0)
            score = 0;
    }

    if (button_state & BUTTON_2) {
        vy = 5;
        if (py <= 0)
            py = 1;
    }

    // Rendering
    dogs_clear();

    draw_rect(t1x, 0, 4, t1o - 10);
    draw_rect(t1x, t1o + 10, 4, DISP_HEIGHT - t1o + 10);
    draw_rect(t2x, 0, 4, t2o - 10);
    draw_rect(t2x, t2o + 10, 4, DISP_HEIGHT - t2o + 10);
    draw_bitmap(4, py, bird);

    draw_number(DISP_WIDTH - 25, DISP_HEIGHT - 8, score);
    dogs_flush();

    // Game logic
    if (t1x == 4)
        score = (py + 2 > t1o - 10 && py + 6 < t1o + 10) ? score + 1 : 0;
    if (t2x == 4)
        score = (py + 2 > t2o - 10 && py + 6 < t2o + 10) ? score + 1 : 0;

    t1x -= 2;
    if (t1x <= -5) {
        t1x = DISP_WIDTH;
	t1o = NEW_BAR();
    }

    t2x -= 2;
    if (t2x <= -5) {
        t2x = DISP_WIDTH;
	t2o = NEW_BAR();
    }

    return 100;
}

