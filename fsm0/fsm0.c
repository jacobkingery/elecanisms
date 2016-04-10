#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"

#define GREEN       0
#define BLUE        1
#define RED         2

uint16_t state, counter;

_LED *green_led, *red_led, *blue_led;

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();

    green_led = &led2;
    red_led = &led1;
    blue_led = &led3;

    timer_setPeriod(&timer1, 0.5);

    state = GREEN;    
    led_on(green_led);

    while (1) {
        switch (state) {
            case GREEN:
                if (sw_read(&sw1) == 0) {
                    state = BLUE;
                    led_off(green_led);
                    led_on(blue_led);
                    timer_start(&timer1);
                    counter = 0;
                }
                break;
            case BLUE:
                if (timer_flag(&timer1)) {
                    timer_lower(&timer1);
                    led_toggle(blue_led);
                    counter++;
                }
                if (sw_read(&sw2) == 0) {
                    state = GREEN;
                    led_off(blue_led);
                    led_on(green_led);
                    timer_stop(&timer1);
                } else if (counter == 20) {
                    state = RED;
                    led_off(blue_led);
                    led_on(red_led);
                    timer_stop(&timer1);
                }
                break;
            case RED:
                break;
        }
    }
}

