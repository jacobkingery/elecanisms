#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"

typedef void (*STATE_HANDLER_T)(void);

void green(void);
void blue(void);
void red(void);

STATE_HANDLER_T state, last_state;
uint16_t counter;

_LED *green_led, *red_led, *blue_led;

void green(void) {
    if (state != last_state) {  // if we are entering the state, do initialization stuff
        last_state = state;
        led_on(green_led);
    }

    // Perform state tasks

    // Check for state transitions
    if (sw_read(&sw1) == 0) {
        state = blue;
    }

    if (state != last_state) {  // if we are leaving the state, do clean up stuff
        led_off(green_led);
    }
}

void blue(void) {
    if (state != last_state) {  // if we are entering the state, do intitialization stuff
        last_state = state;
        led_on(blue_led);
        timer_start(&timer1);
        counter = 0;
    }

    // Perform state tasks
    if (timer_flag(&timer1)) {
        timer_lower(&timer1);
        led_toggle(blue_led);
        counter++;
    }

    // Check for state transitions
    if (sw_read(&sw2) == 0) {
        state = green;
    } else if (counter == 20) {
        state = red;
    }

    if (state != last_state) {  // if we are leaving the state, do clean up stuff
        led_off(blue_led);
        timer_stop(&timer1);
    }
}

void red(void) {
    if (state != last_state) {  // if we are entering the state, do initialization stuff
        state = last_state;
        led_on(red_led);
    }

    // Perform state tasks

    // Check for state transitions

    if (state != last_state) {  // if we are leaving the state, do clean up stuff
        led_off(red_led);
    }
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();

    green_led = &led2;
    red_led = &led1;
    blue_led = &led3;

    timer_setPeriod(&timer1, 0.5);

    state = green;
    last_state = (STATE_HANDLER_T)NULL;

    while (1) {
        state();
    }
}

