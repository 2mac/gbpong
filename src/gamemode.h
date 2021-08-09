/*
 *  gbpong - Pong on the Game Boy
 *  Copyright (C) 2016-2017 David McMackins II
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the Copyfree Open Innovation License, version 0.6 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Copyfree Open Innovation License for more details.
 *
 *  You should have received a copy of the Copyfree Open Innovation License
 *  along with this program.  If not, see <http://coil.apotheon.org>.
 */

#ifndef GBPONG_GAMEMODE_H
#define GBPONG_GAMEMODE_H

struct gamemode
{
	void (*draw)(void);
	void (*finish)(void);
	void (*init)(void);
	void (*update)(void);
};

extern const struct gamemode * const SPLASH_MODE;
extern const struct gamemode * const GAME_MODE;

void
change_gamemode(const struct gamemode *mode);

void
gamemode_cycle(void);

#define NUM_DIFFICULTIES (3)

struct difficulty
{
	uint8_t paddle_size;
	int8_t max_ball_speed;
	int8_t p1speed;
	int8_t p2speed;
};

extern uint8_t winning_score;
extern uint8_t difficulty;
extern const struct difficulty difficulties[NUM_DIFFICULTIES];

#endif
