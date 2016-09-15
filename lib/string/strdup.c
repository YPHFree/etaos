/*
 *  ETA/OS - String duplicate
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <etaos/kernel.h>
#include <etaos/string.h>
#include <etaos/stdlib.h>
#include <etaos/mem.h>
#include <etaos/types.h>

char *strdup(const char *str)
{
	size_t len;
	char *copy;

	len = strlen(str) + 1;
	copy = kzalloc(len);

	if(!copy)
		return NULL;

	memcpy(copy, str, len);
	return copy;
}

