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

static const struct gamemode *active_mode = NULL;

void
change_gamemode (const struct gamemode *mode)
{
  if (active_mode)
    {
      active_mode->finish ();
      gb_wait_vblank ();
      gb_set_lcd_mode (GB_LCD_OFF);
    }

  mode->init ();
  if (active_mode)
    mode->update ();

  active_mode = mode;
}

void
gamemode_cycle ()
{
  if (active_mode)
    {
      active_mode->update ();
      active_mode->draw ();
    }
}
