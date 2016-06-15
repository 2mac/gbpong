/*
 *  gbpong - Pong on the Game Boy
 *  Copyright (C) 2016 David McMackins II
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, version 3 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GBPONG_BUTTON_H
#define GBPONG_BUTTON_H

#include <stdint.h>
#include "bool.h"

struct input
{
  uint8_t code;
  bool pressed;
};

extern struct input * const INPUT_A;
extern struct input * const INPUT_B;
extern struct input * const INPUT_START;
extern struct input * const INPUT_SELECT;
extern struct input * const INPUT_UP;
extern struct input * const INPUT_DOWN;
extern struct input * const INPUT_LEFT;
extern struct input * const INPUT_RIGHT;

bool
button_pressed (struct input *button);

bool
dpad_pressed (struct input *direction);

#endif
