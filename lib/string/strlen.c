/*
 *  ETA/OS - LibC strlen
 *  Copyright (C) 2014   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file strlen.c */

#include <etaos/kernel.h>
#include <etaos/string.h>

/**
 * @addtogroup libc
 * @{
 */

/**
 * @brief Determine the length of a string.
 * @param str String to determine the length of.
 * @return Length of \p str.
 */
size_t strlen(const char *str)
{
	const char *s;

	for(s = str; *s; ++s);
	return (s - str);
}

/** @} */

