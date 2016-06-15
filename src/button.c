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

#include <stdgb.h>
#include "button.h"

enum input_buttons
  {
    A,
    B,
    START,
    SELECT
  };

enum input_dpad
  {
    UP,
    DOWN,
    LEFT,
    RIGHT
  };

static struct input buttons[4] =
  {
    {
      .code = GB_BTN_A,
      .pressed = false
    },

    {
      .code = GB_BTN_B,
      .pressed = false
    },

    {
      .code = GB_BTN_START,
      .pressed = false
    },

    {
      .code = GB_BTN_SELECT,
      .pressed = false
    }
  };

struct input * const INPUT_A = &buttons[A];
struct input * const INPUT_B = &buttons[B];
struct input * const INPUT_START = &buttons[START];
struct input * const INPUT_SELECT = &buttons[SELECT];

static struct input directions[4] =
  {
    {
      .code = GB_DPAD_UP,
      .pressed = false
    },

    {
      .code = GB_DPAD_DOWN,
      .pressed = false
    },

    {
      .code = GB_DPAD_LEFT,
      .pressed = false
    },

    {
      .code = GB_DPAD_RIGHT,
      .pressed = false
    }
  };

struct input * const INPUT_UP = &directions[UP];
struct input * const INPUT_DOWN = &directions[DOWN];
struct input * const INPUT_LEFT = &directions[LEFT];
struct input * const INPUT_RIGHT = &directions[RIGHT];

static bool
input_pressed (uint8_t (*test) (uint8_t), struct input *input)
{
  if (test (input->code))
    {
      if (!input->pressed)
	{
	  input->pressed = true;
	  return true;
	}

      return false;
    }
  else
    {
      input->pressed = false;
      return false;
    }
}

bool
button_pressed (struct input *input)
{
  return input_pressed (gb_button_down, input);
}

bool
dpad_pressed (struct input *input)
{
  return input_pressed (gb_dpad_down, input);
}
