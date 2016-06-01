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
#include "gamemode.h"
#include "text.h"

static uint8_t frames;

static void
splash_init ()
{
  char *screen = GB_SCRN0;
  frames = 0;

  gb_set_all_tile_maps (0);

  screen = &screen[(32 * 5) + 8];
  write_text (screen, "PONG");

  screen = GB_SCRN0;
  screen = &screen[(32 * 15) + 7];
  write_text (screen, COPYRIGHT_STR " 2016");

  screen = &screen[32 - 7 + 1];
  write_text (screen, "DAVID MCMACKINS II");

  gb_set_lcd_mode (GB_LCD_ON | GB_LCD_BGON);
  gb_enable_vblank ();
}

static void
splash_finish ()
{
}

static void
splash_update ()
{
  gb_update_input_state ();
  if (gb_button_down (GB_BTN_START))
    change_gamemode (GAME_MODE);
}

static void
splash_draw ()
{
  char *start_text, *screen = GB_SCRN0;
  screen = &screen[(32 * 12) + 4];

  if (frames++ < 30)
    {
      start_text = "PRESS  START";
    }
  else
    {
      start_text = "            ";
      if (frames > 60)
	frames = 0;
    }

  write_text (screen, start_text);
}

static const struct gamemode splash =
  {
    .init = splash_init,
    .finish = splash_finish,
    .update = splash_update,
    .draw = splash_draw
  };

const struct gamemode * const SPLASH_MODE = &splash;
