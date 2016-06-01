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

#define SCORE1POS (3)
#define SCORE2POS (14)
#define WINNING_SCORE (11)

static char score1buf[4], score2buf[4];
static uint8_t score1, score2;

static void
format_score (uint8_t score, char *buf)
{
  buf[3] = '\0';

  if (0 == score)
    {
      write_text (buf, "  0");
      return;
    }

  for (buf = &buf[2]; score > 0; score /= 10, --buf)
    {
      *buf = (score % 10) + '0';
    }

  while ('0' == *buf)
    *(buf++) = ' ';
}

static void
game_init ()
{
  score1 = 0;
  score2 = 0;

  gb_set_all_tile_maps (0);
  gb_set_lcd_mode (GB_LCD_ON | GB_LCD_BGON | GB_LCD_OBJ8);
  gb_enable_vblank ();
}

static void
game_finish ()
{
}

static void
game_update ()
{
  gb_update_input_state ();

  format_score (score1, score1buf);
  format_score (score2, score2buf);
}

static void
game_draw ()
{
  char *screen = GB_SCRN0;

  gb_update_objects ();

  write_text (&screen[SCORE1POS], score1buf);
  write_text (&screen[SCORE2POS], score2buf);
}

static const struct gamemode game =
  {
    .init = game_init,
    .finish = game_finish,
    .update = game_update,
    .draw = game_draw
  };


const struct gamemode * const GAME_MODE = &game;
