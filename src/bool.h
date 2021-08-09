/*
 *  gbpong - Pong on the Game Boy
 *  Copyright (C) 2016 David McMackins II
 *
 *  This program is free software: you can redistribute and/or modify it under
 *  the terms of the Copyfree Open Innovation License, version 0.6 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Copyfree Open Innovation License for more details.
 *
 *  You should have received a copy of the Copyfree Open Innovation License
 *  along with this program.  If not, see <http://coil.apotheon.org>.
 */

#ifndef GBPONG_BOOL_H
#define GBPONG_BOOL_H

#ifndef INT_BOOLS
# include <stdbool.h>
#else
typedef uint8_t bool;
# define true (1)
# define false (0)
#endif

#endif
