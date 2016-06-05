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

const uint8_t (* const TILES)[GB_BYTES_PER_TILE] = NULL;

void
main (void)
{
  uint8_t i;

  gb_enable_vblank ();
  gb_wait_vblank ();
  gb_set_lcd_mode (GB_LCD_OFF);
  gb_disable_interrupts ();

  gb_set_all_tile_data (0);
  gb_mbc5_select_rombank (1);
  for (i = 0; i < 128; ++i)
    gb_define_tile (i, TILES[i]);

  gb_init_objects ();
  gb_set_bg_palette (0xE4); // 11 10 01 00
  gb_set_object_palette (0, 0xE4);

  change_gamemode (SPLASH_MODE);

  for (;;)
    gamemode_cycle ();
}
