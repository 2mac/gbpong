/*
 *  gbpong - Pong on the Game Boy
 *  Copyright (C) 2016-2017 David McMackins II
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
#include "gamemode.h"
#include "text.h"

#define UP_ARROW_TILE (5)
#define DOWN_ARROW_TILE (6)
#define LEFT_ARROW_TILE (7)
#define RIGHT_ARROW_TILE (8)

enum setting_type
{
	SETTING_DIFFICULTY,
	SETTING_WINSCORE
};

static char winscorebuf[4];
static uint8_t frames;
static uint8_t setting;

static void
splash_init()
{
	char *screen = GB_SCRN0;
	frames = 0;
	setting = SETTING_DIFFICULTY;

	gb_set_all_tile_maps(0);

	screen = &screen[(32 * 3) + 8];
	write_text(screen, "PONG");

	screen = GB_SCRN0;
	screen = &screen[(32 * 6) + 2];
	write_text(screen, "EASY");

	screen = &screen[32];
	write_text(screen, "MEDIUM");

	screen = &screen[32];
	write_text(screen, "HARD");

	screen = GB_SCRN0;
	screen = &screen[(32 * 7) + 10];
	write_text(screen, "WIN:");

	screen = GB_SCRN0;
	screen = &screen[(32 * 15) + 7];
	write_text(screen, COPYRIGHT_STR " 2016");

	screen = GB_SCRN0;
	screen = &screen[(32 * 16) + 1];
	write_text(screen, "DAVID MCMACKINS II");

	gb_set_lcd_mode(GB_LCD_ON | GB_LCD_BGON);
	gb_enable_vblank();
}

static void
splash_finish()
{
}

static void
splash_update()
{
	gb_update_input_state();

	if (button_pressed(INPUT_START))
		change_gamemode(GAME_MODE);

	if (dpad_pressed(INPUT_DOWN))
	{
		if (SETTING_DIFFICULTY == setting)
		{
			if (difficulty < NUM_DIFFICULTIES - 1)
				++difficulty;
		}
		else
		{
			if (0 == --winning_score)
				--winning_score;
		}
	}
	else if (dpad_pressed(INPUT_UP))
	{
		if (SETTING_DIFFICULTY == setting)
		{
			if (difficulty > 0)
				--difficulty;
		}
		else
		{
			if (0 == ++winning_score)
				++winning_score;
		}
	}
	else if (dpad_pressed(INPUT_RIGHT))
	{
		if (SETTING_DIFFICULTY == setting)
			setting = SETTING_WINSCORE;
	}
	else if (dpad_pressed(INPUT_LEFT))
	{
		if (SETTING_WINSCORE == setting)
			setting = SETTING_DIFFICULTY;
	}

	format_byte(winning_score, winscorebuf);
}

static void
splash_draw()
{
	static const uint8_t arrows[2] = { UP_ARROW_TILE, DOWN_ARROW_TILE };
	static const uint8_t spaces[2] = { ' ', ' ' };
	const char *start_text;
	const uint8_t *diff_arrows, *score_arrows;
	char *screen;
	uint8_t i;

	if (SETTING_DIFFICULTY == setting)
	{
		diff_arrows = arrows;
		score_arrows = spaces;
	}
	else
	{
		score_arrows = arrows;
		diff_arrows = spaces;
	}

	if (frames++ < 30)
	{
		start_text = "PRESS  START";
	}
	else
	{
		start_text = "            ";
		if (frames >= 60)
			frames = 0;
	}

	gb_wait_vblank();

	screen = GB_SCRN0;
	screen = &screen[(32 * 5) + 4];
	screen[0] = diff_arrows[0];
	screen[32 * (NUM_DIFFICULTIES + 1)] = diff_arrows[1];

	screen = GB_SCRN0;
	for (screen = &screen[(32 * 6) + 1], i = 0; i < NUM_DIFFICULTIES;
	     ++i, screen = &screen[32])
	{
		screen[0] = ((difficulty == i) ? RIGHT_ARROW_TILE : ' ');
	}

	screen = GB_SCRN0;
	screen = &screen[(32 * 7) + 15];
	write_text(screen, winscorebuf);

	screen = &screen[2 - 32];
	screen[0] = score_arrows[0];
	screen[32 * 2] = score_arrows[1];

	screen = GB_SCRN0;
	screen = &screen[(32 * 12) + 4];
	write_text(screen, start_text);
}

static const struct gamemode splash =
{
	.init = splash_init,
	.finish = splash_finish,
	.update = splash_update,
	.draw = splash_draw
};

const struct gamemode * const SPLASH_MODE = &splash;
