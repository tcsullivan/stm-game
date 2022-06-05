#include "buttons.h"
#include "dogs.h"

static int values[16];

void g2048_init()
{
    for (int i = 0; i < 16; i++)
        values[i] = 0;
}

int g2048_loop()
{
    if ((button_state & BUTTON_JOYUP) == BUTTON_JOYUP) {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {

                if (values[4 * y + x] != 0)
                    continue;
            }
        }

        for (int y = 3; y > 0; y--) {
            for (int x = 0; x < 4; x++) {
                if (values[4 * y + x] != 0) {
                    if (values[4 * (y - 1) + x] == 0)
                        values[4 * (y - 1) + x] = values[4 * y + x], values[4 * y + x] = 0;
                    else if (values[4 * (y - 1) + x] == values[4 * y + x])
                        values[4 * (y - 1) + x]++, values[4 * y + x] = 0;
                }
            }
        }

        for (int i = 15; i >= 0; i--) {
            if (values[i] == 0) {
                values[i] = 1;
                break;
            }
        }
    } else if ((button_state & BUTTON_JOYDOWN) == BUTTON_JOYDOWN) {
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 4; x++) {
                if (values[4 * y + x] != 0) {
                    if (values[4 * (y + 1) + x] == 0)
                        values[4 * (y + 1) + x] = values[4 * y + x], values[4 * y + x] = 0;
                    else if (values[4 * (y + 1) + x] == values[4 * y + x])
                        values[4 * (y + 1) + x]++, values[4 * y + x] = 0;
                }
            }
        }

        for (int i = 15; i >= 0; i--) {
            if (values[i] == 0) {
                values[i] = 1;
                break;
            }
        }
    } else if ((button_state & BUTTON_JOYLEFT) == BUTTON_JOYLEFT) {
        for (int x = 3; x > 0; x--) {
            for (int y = 0; y < 4; y++) {
                if (values[4 * y + x] != 0) {
                    if (values[4 * y + x - 1] == 0)
                        values[4 * y + x - 1] = values[4 * y + x], values[4 * y + x] = 0;
                    else if (values[4 * y + x - 1] == values[4 * y + x])
                        values[4 * y + x - 1]++, values[4 * y + x] = 0;
                }
            }
        }

        for (int i = 15; i >= 0; i--) {
            if (values[i] == 0) {
                values[i] = 1;
                break;
            }
        }
    } else if ((button_state & BUTTON_JOYRIGHT) == BUTTON_JOYRIGHT) {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 4; y++) {
                if (values[4 * y + x] != 0) {
                    if (values[4 * y + x + 1] == 0)
                        values[4 * y + x + 1] = values[4 * y + x], values[4 * y + x] = 0;
                    else if (values[4 * y + x + 1] == values[4 * y + x])
                        values[4 * y + x + 1]++, values[4 * y + x] = 0;
                }
            }
        }

        for (int i = 15; i >= 0; i--) {
            if (values[i] == 0) {
                values[i] = 1;
                break;
            }
        }
    }

    dogs_clear();
    for (int i = 0; i < 16; i++) {
        if (values[i] != 0)
            draw_number(31 + 10 * (i % 4), 12 + 10 * (3 - (i / 4)), values[i]);
    }
    dogs_flush();

    return 500;
}
