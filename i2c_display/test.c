#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "i2c.h"

uint8_t numbertable[] = {
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F, /* 9 */
    0x77, /* a */
    0x7C, /* b */
    0x39, /* C */
    0x5E, /* d */
    0x79, /* E */
    0x71, /* F */
};

void command_display(uint8_t addr, uint8_t cmd) {
    addr = (addr << 1) | 0;     // Write mode
    i2c_start(&i2c1);
    i2c_putc(&i2c1, addr);
    i2c_putc(&i2c1, cmd);
    i2c_stop(&i2c1);
}

void init_display(uint8_t addr) {
    // Turn on system oscillator
    command_display(addr, 0x21);
    // Set blinking to off, display to on
    command_display(addr, 0x81);
    // Set brightness to max
    command_display(addr, 0xEF);
}

void write_display(uint8_t addr, uint16_t value, uint8_t colon) {
    addr = (addr << 1) | 0;     // Write mode
    i2c_start(&i2c1);
    i2c_putc(&i2c1, addr);
    i2c_putc(&i2c1, 0x00);      // Set data address pointer to 0

    i2c_putc(&i2c1, numbertable[value / 1000]);         // Thousands
    i2c_putc(&i2c1, 0x00);
    i2c_putc(&i2c1, numbertable[(value / 100) % 10]);   // Hundreds
    i2c_putc(&i2c1, 0x00);
    i2c_putc(&i2c1, 0x02 & (colon << 1));               // Colon
    i2c_putc(&i2c1, 0x00);
    i2c_putc(&i2c1, numbertable[(value / 10) % 10]);    // Tens
    i2c_putc(&i2c1, 0x00);
    i2c_putc(&i2c1, numbertable[value % 10]);           // Ones
    i2c_putc(&i2c1, 0x00);

    i2c_stop(&i2c1);
}

int16_t main(void) {
    init_clock();
    init_i2c();

    // Initialize i2c interface
    i2c_open(&i2c1, 4e5);
    uint8_t i2c_addr0 = 0x70;
    uint8_t i2c_addr1 = 0x71;
    init_display(i2c_addr0);
    init_display(i2c_addr1);

    uint16_t number = 0;
    while (1) {
        write_display(i2c_addr0, 9999 - number, (9999 - number) % 1000 / 500);
        write_display(i2c_addr1, number++, number % 1000 / 500);
        number = (number > 9999) ? 0 : number;
    }
}
