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

#ifndef GBPONG_GAMEMODE_H
#define GBPONG_GAMEMODE_H

struct gamemode
{
  void (*draw) (void);
  void (*finish) (void);
  void (*init) (void);
  void (*update) (void);
};

extern const struct gamemode * const SPLASH_MODE;
extern const struct gamemode * const GAME_MODE;

void
change_gamemode (const struct gamemode *mode);

void
gamemode_cycle (void);

#endif
