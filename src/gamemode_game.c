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

#define BALL_TILE (2)
#define PADDLE_TILE (1)
#define LEFT_MID_TILE (3)
#define RIGHT_MID_TILE (4)

#define BALL_SIZE (6)
#define LEFT_PADDLE_X (20)
#define RIGHT_PADDLE_X (148)
#define MIN_PADDLE_POS (25)
#define MAX_PADDLE_POS (117)
#define SCORE1POS (3)
#define SCORE2POS (14)

#ifndef INT_BOOLS
# include <stdbool.h>
#else
typedef uint8_t bool;
# define true (1)
# define false (0)
#endif

static uint8_t (*p1objs)[GB_BYTES_PER_OBJ];
static uint8_t (*p2objs)[GB_BYTES_PER_OBJ];
static uint8_t *ballobj;
static char score1buf[4], score2buf[4];
static uint8_t delay_state, half_paddle_height, paddle_height;
static uint8_t score1, score2, pos1, pos2;
static int16_t ball_x, ball_y;
static int8_t ball_x_speed, ball_y_speed, ball_x_start_speed;

uint8_t paddle_size = 4;
uint8_t winning_score = 11;
int8_t min_ball_speed;
int8_t max_ball_speed = 2;
int8_t speed1 = 2;
int8_t speed2 = 1;

static void
format_score (uint8_t score, char *buf)
{
  buf[3] = '\0';
  write_text (buf, "   ");
  buf = &buf[2];

  do
    {
      *(buf--) = (score % 10) + '0';
      score /= 10;
    }
  while (score);
}

static void
init_paddle (uint8_t (*paddle)[GB_BYTES_PER_OBJ], uint8_t x_pos)
{
  uint8_t i;

  for (i = 0; i < paddle_size; ++i)
    {
      paddle[i][GB_OBJ_YPOS] = 16 + 72 - half_paddle_height + (8 * i);
      paddle[i][GB_OBJ_XPOS] = x_pos;
      paddle[i][GB_OBJ_TILE] = PADDLE_TILE;
    }
}

static void
reset_ball ()
{
  ball_x = 80;
  ball_y = 72;
  ball_x_speed = ball_x_start_speed;
  ball_y_speed = 0;
}

static void
reset_positions ()
{
  pos1 = 72;
  pos2 = 72;
  init_paddle (p1objs, LEFT_PADDLE_X);
  init_paddle (p2objs, RIGHT_PADDLE_X);

  reset_ball ();
}

static void
game_init ()
{
  uint8_t i;
  char *screen = GB_SCRN0;

  gb_init_objects ();

  p1objs = GB_OBJECTS;
  p2objs = &p1objs[paddle_size];
  ballobj = (void *) &p2objs[paddle_size];
  ballobj[GB_OBJ_TILE] = BALL_TILE;

  min_ball_speed = -max_ball_speed;
  score1 = 0;
  score2 = 0;
  delay_state = 1;
  half_paddle_height = paddle_size * 4;
  paddle_height = half_paddle_height * 2;
  ball_x_start_speed = -1;
  reset_positions ();

  gb_set_all_tile_maps (0);
  for (i = GB_LCD_Y_BYTE, screen = &screen[9]; i > 0;
       --i, screen = &screen[32])
    {
      screen[0] = LEFT_MID_TILE;
      screen[1] = RIGHT_MID_TILE;
    }

  gb_set_lcd_mode (GB_LCD_ON | GB_LCD_BGON | GB_LCD_OBJ8);
  gb_enable_vblank ();
}

static void
game_finish ()
{
}

static void
move_paddle (uint8_t *pos, uint8_t (*objs)[GB_BYTES_PER_OBJ], int8_t magnitude)
{
  uint8_t try = *pos + magnitude;

  if (try < MIN_PADDLE_POS)
    magnitude += MIN_PADDLE_POS - try;
  else if (try > MAX_PADDLE_POS)
    magnitude -= try - MAX_PADDLE_POS;

  *pos += magnitude;
  for (try = 0; try < paddle_size; ++try)
    objs[try][GB_OBJ_YPOS] += magnitude;
}

static void
score ()
{
  delay_state = 122;
  reset_ball ();
}

static bool
same_sign (int16_t x, int16_t y)
{
  return (x >= 0 && y >= 0) || (x < 0 && y < 0);
}

static void
inc_ball_y_speed ()
{
  if (ball_y_speed >= 0)
    ++ball_y_speed;
  else
    --ball_y_speed;
}

static void
cap_ball_speed (int8_t *speed)
{
  if (*speed > max_ball_speed)
    *speed = max_ball_speed;
  else if (*speed < min_ball_speed)
    *speed = min_ball_speed;
}

static void
game_update ()
{
  int8_t inverted_x = -ball_x_speed, inverted_y = -ball_y_speed;
  int8_t move1 = 0, move2 = 0;
  int16_t top_edge, bottom_edge, left_edge, right_edge;

  switch (delay_state)
    {
    case 2:
      for (; delay_state < 122; ++delay_state)
	gb_wait_vblank ();

      ball_x_start_speed *= -1;
      reset_positions ();
      score1 = 0;
      score2 = 0;
      delay_state = 0;
      break;

    case 123:
      for (; delay_state < 183; ++delay_state)
	gb_wait_vblank ();

      delay_state = 0;
      break;

    case 184:
      do
	{
	  gb_wait_vblank ();
	  gb_update_input_state ();
	}
      while (!gb_button_down (GB_BTN_START));

      do
	{
	  gb_wait_vblank ();
	  gb_update_input_state ();
	}
      while (gb_button_down (GB_BTN_START));

      delay_state = 0;
      break;

    default:
      break;
    }

  gb_update_input_state ();

  if (gb_dpad_down (GB_DPAD_DOWN) && pos1 < MAX_PADDLE_POS)
    move1 = speed1;
  else if (gb_dpad_down (GB_DPAD_UP) && pos1 > MIN_PADDLE_POS)
    move1 = -speed1;

  if (ball_y > pos2 && pos2 < MAX_PADDLE_POS)
    move2 = speed2;
  else if (ball_y < pos2 && pos2 > MIN_PADDLE_POS)
    move2 = -speed2;

  move_paddle (&pos1, p1objs, move1);
  move_paddle (&pos2, p2objs, move2);

  ball_x += ball_x_speed;
  ball_y += ball_y_speed;
  top_edge = ball_y - (BALL_SIZE / 2);
  bottom_edge = ball_y + (BALL_SIZE / 2);
  left_edge = ball_x - (BALL_SIZE / 2);
  right_edge = ball_x + (BALL_SIZE / 2);

  if (ball_y_speed < 0)
    {
      if (ball_y <= 0)
	ball_y_speed = inverted_y;
    }
  else
    {
      if (ball_y >= 144)
	ball_y_speed = inverted_y;
    }

  if (ball_x_speed < 0)
    {
      int16_t ox = LEFT_PADDLE_X - left_edge;
      int16_t oy;

      if (ball_x <= 0)
	{
	  score ();
	  ++score2;
	  ball_x_speed = 1;
	}
      else if (ox >= 0 && ox <= 8)
	{
	  if (ball_y <= pos1)
	    oy = bottom_edge - (pos1 - half_paddle_height);
	  else
	    oy = (pos1 + half_paddle_height) - top_edge;

	  if (oy >= 0)
	    {
	      if (ox <= oy)
		{
		  ball_x_speed = inverted_x + 1;

		  if (move1)
		    {
		      if (0 == ball_y_speed)
			ball_y_speed = move1;
		      else if (same_sign (ball_y_speed, move1))
			inc_ball_y_speed ();
		      else
			ball_y_speed = inverted_y + 1;
		    }
		}

	      if (ox >= oy)
		ball_y_speed = inverted_y + 1;
	    }
	}
    }
  else
    {
      int16_t ox = right_edge - (RIGHT_PADDLE_X - 8);
      int16_t oy;

      if (ball_x >= 160)
	{
	  score ();
	  ++score1;
	  ball_x_speed = -1;
	}
      else if (ox >= 0 && ox <= 8)
	{
	  if (ball_y <= pos2)
	    oy = bottom_edge - (pos2 - half_paddle_height);
	  else
	    oy = (pos2 + half_paddle_height) - top_edge;

	  if (oy >= 0)
	    {
	      if (ox <= oy)
		{
		  ball_x_speed = inverted_x - 1;

		  if (move2)
		    {
		      if (0 == ball_y_speed)
			ball_y_speed = move2;
		      else if (same_sign (ball_y_speed, move2))
			inc_ball_y_speed ();
		      else
			ball_y_speed = inverted_y + 1;
		    }
		}

	      if (ox >= oy)
		ball_y_speed = inverted_y + 1;
	    }
	}
    }

  cap_ball_speed (&ball_x_speed);
  cap_ball_speed (&ball_y_speed);

  ballobj[GB_OBJ_XPOS] = 8 + ball_x - (BALL_SIZE / 2);
  ballobj[GB_OBJ_YPOS] = 16 + ball_y - (BALL_SIZE / 2);

  while (gb_button_down (GB_BTN_START))
    {
      gb_wait_vblank ();
      gb_update_input_state ();
      delay_state = 183;
    }

  if (score1 >= winning_score || score2 >= winning_score)
    delay_state = 1;

  format_score (score1, score1buf);
  format_score (score2, score2buf);
}

static void
game_draw ()
{
  char *screen = GB_SCRN0;

  if (delay_state)
    ++delay_state;

  gb_wait_vblank ();
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
