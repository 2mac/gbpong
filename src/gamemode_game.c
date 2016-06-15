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
#include "bool.h"
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
#define STARTING_PADDLE_POS (72)
#define SCORE1POS (3)
#define SCORE2POS (14)

struct player
{
  uint8_t (*objs)[GB_BYTES_PER_OBJ];
  uint8_t x_pos;
  uint8_t y_pos;
  int8_t speed;
  int8_t vel;
};

static struct player p1, p2;
static char score1buf[4], score2buf[4];
static uint8_t *ballobj;
static uint8_t delay_state, half_paddle_height, paddle_height;
static uint8_t score1, score2;
static uint8_t paddle_size;
static int16_t ball_x, ball_y, top_edge, bottom_edge;
static int8_t ball_x_speed, ball_y_speed, ball_x_start_speed;
static int8_t inverted_x, inverted_y;
static int8_t min_ball_speed, max_ball_speed;

static void
init_paddle (struct player *p)
{
  uint8_t i;

  p->y_pos = STARTING_PADDLE_POS;

  for (i = 0; i < paddle_size; ++i)
    {
      p->objs[i][GB_OBJ_YPOS] = 16 + 72 - half_paddle_height + (8 * i);
      p->objs[i][GB_OBJ_XPOS] = p->x_pos;
      p->objs[i][GB_OBJ_TILE] = PADDLE_TILE;
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
  init_paddle (&p1);
  init_paddle (&p2);

  reset_ball ();
}

static void
game_init ()
{
  uint8_t i;
  char *screen = GB_SCRN0;
  const struct difficulty *d = &difficulties[difficulty];

  gb_init_objects ();

  p1.speed = d->p1speed;
  p1.x_pos = LEFT_PADDLE_X;
  p2.speed = d->p2speed;
  p2.x_pos = RIGHT_PADDLE_X;

  paddle_size = d->paddle_size;
  max_ball_speed = d->max_ball_speed;
  min_ball_speed = -max_ball_speed;

  p1.objs = GB_OBJECTS;
  p2.objs = &p1.objs[paddle_size];
  ballobj = (void *) &p2.objs[paddle_size];
  ballobj[GB_OBJ_TILE] = BALL_TILE;

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
move_paddle (struct player *p, int8_t magnitude)
{
  uint8_t try = p->y_pos + magnitude;

  if (try < MIN_PADDLE_POS)
    magnitude += MIN_PADDLE_POS - try;
  else if (try > MAX_PADDLE_POS)
    magnitude -= try - MAX_PADDLE_POS;

  p->y_pos += magnitude;
  for (try = 0; try < paddle_size; ++try)
    p->objs[try][GB_OBJ_YPOS] += magnitude;
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
inc_abs_value (int8_t *p)
{
  *p = (*p >= 0) ? *p + 1 : *p - 1;
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
check_collision (struct player *p, int16_t ox)
{
  int16_t oy;

  if (ox >= 0 && ox <= 8)
    {
      if (ball_y <= p->y_pos)
	oy = bottom_edge - (p->y_pos - half_paddle_height);
      else
	oy = (p->y_pos + half_paddle_height) - top_edge;

      if (oy >= 0)
	{
	  inc_abs_value (&inverted_y);

	  if (ox <= oy)
	    {
	      ball_x_speed = inverted_x;
	      inc_abs_value (&ball_x_speed);

	      if (p->vel)
		{
		  if (0 == ball_y_speed)
		    ball_y_speed = p->vel;
		  else if (same_sign (ball_y_speed, p->vel))
		    inc_abs_value (&ball_y_speed);
		  else
		    ball_y_speed = inverted_y;
		}
	    }

	  if (ox >= oy && !same_sign (ball_y_speed, p->vel))
	    ball_y_speed = inverted_y;
	}
    }
}

static void
game_update ()
{
  int16_t left_edge, right_edge;

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

  p1.vel = 0;
  p2.vel = 0;
  inverted_x = -ball_x_speed;
  inverted_y = -ball_y_speed;

  gb_update_input_state ();

  if (gb_dpad_down (GB_DPAD_DOWN) && p1.y_pos < MAX_PADDLE_POS)
    p1.vel = p1.speed;
  else if (gb_dpad_down (GB_DPAD_UP) && p1.y_pos > MIN_PADDLE_POS)
    p1.vel = -p1.speed;

  if (ball_y > p2.y_pos && p2.y_pos < MAX_PADDLE_POS)
    p2.vel = p2.speed;
  else if (ball_y < p2.y_pos && p2.y_pos > MIN_PADDLE_POS)
    p2.vel = -p2.speed;

  move_paddle (&p1, p1.vel);
  move_paddle (&p2, p2.vel);

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
      if (ball_x <= 0)
	{
	  score ();
	  ++score2;
	  ball_x_speed = 1;
	}
      else
	{
	  check_collision (&p1, LEFT_PADDLE_X - left_edge);
	}
    }
  else
    {
      if (ball_x >= 160)
	{
	  score ();
	  ++score1;
	  ball_x_speed = -1;
	}
      else
	{
	  check_collision (&p2, right_edge - (RIGHT_PADDLE_X - 8));
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

  format_byte (score1, score1buf);
  format_byte (score2, score2buf);
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
