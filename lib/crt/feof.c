/*
 *  ETA/OS - LibC feof
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
 * @addtogroup libcio
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/bitops.h>
#include <etaos/error.h>
#include <etaos/stdio.h>

/**
 * @brief Check if an EOF event has occurred.
 * @param file File to check.
 * @return 0 if no EOF was found yet.
 */
int feof(FILE *file)
{
	if(!file)
		return -EBADF;

	return test_bit(STREAM_EOF_FLAG, &file->flags);
}

/** @} */

