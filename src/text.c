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

#include <stdint.h>
#include "text.h"

void
format_byte(uint8_t byte, char *buf)
{
	buf[3] = '\0';
	write_text(buf, "   ");
	buf = &buf[2];

	do
	{
		*(buf--) = (byte % 10) + '0';
		byte /= 10;
	} while (byte);
}

void
write_text(char *dest, const char *src)
{
	while (*src != '\0')
		*(dest++) = *(src++);
}
