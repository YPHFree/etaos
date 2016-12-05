/*
 *  ETA/OS - String copy
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

/**
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/string.h>
#include <etaos/types.h>

/**
 * @brief Copy a number of bytes of string.
 * @param dst Destination string.
 * @param src Source string.
 * @param num Number of bytes to copy.
 * @return The destination string.
 *
 * Copy the length of \p src bytes or \p num bytes (whichever is smaller) into
 * \p dst.
 */
char *strncpy(char *dst, const char *src, size_t num)
{
	char *rv = dst;

	if(!num)
		return rv;

	do {
		if((*dst++ = *src++) == '\0')
			break;
	} while(--num);

	return rv;
}

/** @} */

