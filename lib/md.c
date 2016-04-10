/*
** Copyright (c) 2016, Evan Dorsky
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**     2. Redistributions in binary form must reproduce the above copyright
**        notice, this list of conditions and the following disclaimer in the
**        documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/
#include <p24FJ128GB206.h>
#include "common.h"
#include "md.h"

_MD md1, md2;

void __md_setpins(_MD *self) {
    if (!self->braked) {
        pin_write(self->pins[self->dir], self->speed);
        pin_clear(self->pins[!self->dir]);
    } else {
        pin_write(self->pins[self->dir], self->brakeType);
        pin_write(self->pins[!self->dir], self->brakeType);
    }
}


void init_md(void) {
    md_init(&md1, &D[7], &D[8], 500, &oc7);
    md_init(&md2, &D[6], &D[5], 500, &oc5);
}

void md_init(_MD *self, _PIN *pin1, _PIN *pin2, uint16_t freq, _OC *oc) {
    self->dir = 0;
    self->speed = 0;
    self->freq = freq;
    self->pins[0] = pin1; // M+
    self->pins[1] = pin2; // M-
    self->braked = 0;
    self->brakeType = 0;
    self->oc = oc;

    oc_pwm(self->oc, self->pins[self->dir], &timer5, freq, 0);
    pin_clear(self->pins[!self->dir]);
}

void md_free(_MD *self) {
    // clear owned pins/oc's
}

void md_brake(_MD *self) {
    if (!self->braked) {
        self->braked = 1;

        oc_free(self->oc);

        __md_setpins(self);
    }
}

void md_run(_MD *self) {
    if (self->braked) {
        self->braked = 0;

        oc_pwm(self->oc, self->pins[self->dir], &timer5, self->freq, 0);

        __md_setpins(self);
    }
}

void md_brakeType(_MD *self, uint8_t type) {
    self->brakeType = type;
}

void md_speed(_MD *self, uint16_t speed) {
    self->speed = speed;
    __md_setpins(self);
}

void md_direction(_MD *self, uint8_t dir) {
    if (self->dir == dir) {
        return;
    }
    self->dir = dir;

    oc_free(self->oc);
    oc_pwm(self->oc, self->pins[dir], &timer5, self->freq, 0);

    __md_setpins(self);
}

void md_velocity(_MD *self, uint16_t speed, uint8_t dir) {
    md_speed(self, speed);
    md_direction(self, dir);
}
